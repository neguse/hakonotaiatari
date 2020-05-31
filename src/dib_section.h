
#pragma once

class DIBSection {

private:
	u32 m_width, m_height;
	u32* m_pBits;
	HBITMAP m_hBitmap, m_hPrevBmp;
	HDC m_hMemDC;

public:

	DIBSection();
	~DIBSection();

	Retval initialize(HWND hWnd, u32 w, u32 h);

	Retval finalize();

	bool isInitialized() const 
	{
		return m_hBitmap != NULL;
	}

	u32 getWidth() const
	{
		return m_width;
	}

	u32 getHeight() const
	{
		return m_height;
	}

	void setPixel(u32 x, u32 y, u32 color)
	{
		bool isOutOfArea = x >= m_width || y >= m_height;
		CHECK(!isOutOfArea) << "setPixelが領域範囲外で呼ばれました";
		if (isOutOfArea) return; // TODO: 適宜なおす
		m_pBits[x + y * m_width] = color;
	}

	u32 getPixel(u32 x, u32 y)
	{
		return m_pBits[x + y * m_width];
	}

	HDC getDDB() const
	{
		return m_hMemDC;
	}

	u32* getBufferAddress() const
	{
		return m_pBits;
	}

}; // class DIBSection
