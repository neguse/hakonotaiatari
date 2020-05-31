
#include "precompile.h"
#include "dib_section.h"

// コンストラクタ
DIBSection::DIBSection()
	: m_pBits(NULL)
	, m_hBitmap(NULL)
	, m_hPrevBmp(NULL)
	, m_hMemDC(NULL)
{

}

// デストラクタ
DIBSection::~DIBSection()
{
	finalize();
}

// 初期化
Retval DIBSection::initialize(HWND hWnd, u32 w, u32 h)
{
	if (isInitialized()) {
		return Retval(R_FAIL_MULTIPLE_INITIALIZE);
	}

	// setup bitmapinfo
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(bmi));
	{
		BITMAPINFOHEADER& head = bmi.bmiHeader;
		head.biSize = sizeof(BITMAPINFOHEADER);
		head.biBitCount = 32;
		head.biPlanes = 1;
		head.biWidth = w;
		head.biHeight = -h;
	}

	// create DIBSection
	m_hBitmap = CreateDIBSection(
		NULL,
		&bmi,
		DIB_RGB_COLORS,
		(void**)&m_pBits,
		NULL,
		0);
	if (NULL == m_hBitmap) {
		TraceWin32Error();
		return Retval(R_FAIL);
	}
	
	// TODO: ここのエラーハンドリングを丁寧にやる
	HDC hDC = GetDC(hWnd);
	m_hMemDC = CreateCompatibleDC(hDC);
	ReleaseDC(hWnd, hDC);
	m_hPrevBmp = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);

	m_width = w;
	m_height = h;

	return Retval(R_SUCCESS);
}

Retval DIBSection::finalize()
{
	if (!isInitialized()) {
		return Retval(R_FAIL_MULTIPLE_FINALIZE);
	}

	// TODO: ここのエラーハンドリングを丁寧にやる

	SelectObject(m_hMemDC, m_hPrevBmp);
	DeleteObject(m_hBitmap);

	return Retval(R_SUCCESS);

	m_hBitmap = NULL;
	m_hMemDC = NULL;
}
