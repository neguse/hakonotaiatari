
#include "precompile.h"
#include "../fs1/resource.h"
#include "window.h"

namespace {

	class WindowClass
	{
	public:
		WNDCLASSEX m_wc;
	public:
		WindowClass(WNDPROC handler)
		{

			WNDCLASSEX wc = {
				sizeof( WNDCLASSEX ), CS_CLASSDC, handler, 0L, 0L,
				::GetModuleHandle(NULL),
				LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)),
				LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)), NULL, NULL,
				"NGL1", NULL
			};
			m_wc = wc;
			ATOM ret = ::RegisterClassEx(&m_wc);
			if (ret == 0) {
				TraceWin32Error();
			}
		}
		~WindowClass() {
			BOOL ret = ::UnregisterClass( "NGL1", m_wc.hInstance );
			if (ret == 0) {
				TraceWin32Error();
			}
		}

	}; // WindowClass

	static bool getDisplayResolution(glm::u32vec2& outResolution)
	{
		DEVMODE devmode;
		ZeroMemory(&devmode, sizeof(devmode));
		if (0 != ::EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode)) {
			outResolution.x = devmode.dmPelsWidth;
			outResolution.y = devmode.dmPelsHeight;
			return true;
		} else {
			TraceWin32Error();
			LOG(ERROR) << "EnumDisplaySettings()が0を返しました";
			return false;
		}
	}

	static const u32 WINDOW_STYLE_WINDOWED = WS_POPUP | WS_CAPTION | WS_SYSMENU;
	static const u32 WINDOW_STYLE_FULLSCREEN = WS_POPUP;

} // namespace


Window::Window()
: m_hWindow(NULL)
, m_isQuit(false)
, m_isFullScreen(false)
{
}

Retval Window::initialize(int width, int height)
{
	static WindowClass cls(handler);

	if (m_hWindow != NULL) {
		return Retval(R_FAIL_MULTIPLE_INITIALIZE);
	}

	RECT rect;
	{
		rect.left = rect.top = 0;
		rect.right = width;
		rect.bottom = height;
		BOOL ret = ::AdjustWindowRect(&rect, WINDOW_STYLE_WINDOWED, false);
		if (ret == 0) {
			TraceWin32Error();
			return Retval(R_FAIL);
		}
	}

	{
		m_hWindow = ::CreateWindow( "NGL1", "hakotai 1.0(C82 ver.)",
			WINDOW_STYLE_WINDOWED, 200, 200, rect.right - rect.left, rect.bottom - rect.top,
				NULL, NULL, cls.m_wc.hInstance, NULL );
		if (m_hWindow == NULL) {
			TraceWin32Error();
			return Retval(R_FAIL);
		}
	}

	{
		::SetWindowLongPtr(m_hWindow, GWLP_USERDATA, (LONG_PTR)this);
	}

	m_setWidth = width;
	m_setHeight = height;
	m_currentWidth = width;
	m_currentHeight = height;

	clearLButtonCount();


	// TODO: 失敗時にGetLastErrorをログする
	::ShowWindow(m_hWindow, SW_SHOWDEFAULT);
	::UpdateWindow(m_hWindow);

	return Retval(R_SUCCESS);
}

Retval Window::finalize()
{
	if (m_hWindow == NULL) {
		return Retval(R_FAIL_MULTIPLE_FINALIZE);
	}

	if (!m_isQuit) {
		BOOL ret = ::DestroyWindow(m_hWindow);
		if (ret == 0) {
			TraceWin32Error();
			return Retval(R_FAIL);
		}
	}

	m_hWindow = NULL;
	m_isQuit = true;
	return Retval(R_SUCCESS);
}

bool Window::setFullScreen(bool isFull)
{
	ShowWindow(m_hWindow, SW_HIDE);

	u32 style;
	u32 ex_style;

	if (isFull) {
		style = WINDOW_STYLE_FULLSCREEN;
		ex_style = WS_EX_TOPMOST;
	} else {
		style = WINDOW_STYLE_WINDOWED;
		ex_style = 0;
	}

	u32 x, y, width, height;
	if (isFull) {
		glm::u32vec2 res;
		if (!getDisplayResolution(res)) {
			LOG(ERROR) << "ディスプレイ解像度の取得に失敗しました";
		}
		width = res.x; height = res.y;
		x = y = 0;
	} else {
		width = m_setWidth; height = m_setHeight;
		x = y = 200;
	}

	RECT rect;
	{
		rect.left = rect.top = 0;
		rect.right = width;
		rect.bottom = height;
		BOOL ret = ::AdjustWindowRectEx(&rect, style, false, ex_style);
		if (ret == 0) {
			TraceWin32Error();
			return false;
		}
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	SetWindowLong(m_hWindow, GWL_STYLE, style);
	SetWindowLong(m_hWindow, GWL_EXSTYLE, ex_style);
	SetWindowPos(m_hWindow, NULL, x, y, width, height, 0);

	ShowWindow(m_hWindow, SW_SHOW);
	m_isFullScreen = isFull;

	HDC hWinDC = GetDC(m_hWindow);
	CHECK(hWinDC != NULL);
	BitBlt(hWinDC, 0, 0, width, height, NULL, 0, 0, BLACKNESS);
	ReleaseDC(m_hWindow, hWinDC);

	m_currentWidth = width;
	m_currentHeight = height;
	m_lbuttonUpCount = m_lbuttonDownCount = 0;
	return true;
}

vec2 Window::getWindowSize() const
{
	RECT rect;
	::GetClientRect(m_hWindow, &rect);
	return vec2(rect.right - rect.left, rect.bottom - rect.top);
}

LRESULT WINAPI Window::handler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* pWindow = (Window*) ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch( msg )
	{
		case WM_DESTROY:
			::PostQuitMessage( 0 );
			return 0;
		case WM_SETCURSOR:
			break;		
		case WM_ACTIVATE:
			{
				u32 fActive = LOWORD(wParam);
				if (NULL != pWindow) {
					pWindow->m_isActive = fActive != WA_INACTIVE;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			if (pWindow != NULL) {
				pWindow->m_lbuttonDownCount++;
			}
			break;
		case WM_LBUTTONUP:
			if (pWindow != NULL) {
				pWindow->m_lbuttonUpCount++;
			}
			break;
		case WM_KEYDOWN:
			if (wParam == 'F' && (lParam & 0x40000000) == 0) {
				if (pWindow != NULL) {
					pWindow->setFullScreen(!pWindow->isFullScreen());
				}
			}
			break;
		case WM_SYSKEYDOWN:
			if (wParam == VK_RETURN) {
				if (pWindow != NULL) {
					pWindow->setFullScreen(!pWindow->isFullScreen());
				}
			}
			break;
	}

	return ::DefWindowProc( hWnd, msg, wParam, lParam );

}

void Window::doEvents()
{
	MSG msg;
	while ( ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
		if (msg.message == WM_QUIT) {
			m_isQuit = true;
			break;
		}
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}
}

