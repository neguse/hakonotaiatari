
#pragma once

class Window {
private:

	HWND m_hWindow;
	u32 m_style;
	bool m_isQuit;
	bool m_isActive;
	u32 m_setWidth;
	u32 m_setHeight;
	u32 m_currentWidth;
	u32 m_currentHeight;
	bool m_isFullScreen;
	u32 m_lbuttonDownCount;
	u32 m_lbuttonUpCount;

public:

	Window();

	Retval initialize(int width, int height);
	Retval finalize();

	u32 getLButtonDownCount()
	{
		return m_lbuttonDownCount;
	}
	u32 getLButtonUpCount()
	{
		return m_lbuttonUpCount;
	}
	void clearLButtonCount()
	{
		m_lbuttonUpCount = m_lbuttonDownCount = 0;
	}

	bool setFullScreen(bool isFull);
	bool isFullScreen() const
	{
		return m_isFullScreen;
	}

	HWND getHandle() const
	{
		return m_hWindow;
	}

	bool isQuit() const
	{
		return m_isQuit;
	}

	bool isActive() const
	{
		return m_isActive;
	}

	vec2 getWindowSize() const;

	void doEvents();

	static LRESULT WINAPI handler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

}; // class Window

