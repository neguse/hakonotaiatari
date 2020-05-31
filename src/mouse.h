
#pragma once

class Window;

class Mouse {

public:
	static const int MOUSE_BUTTON_MAX = 3;

private:

	Window* m_pWindow;

	bool m_lbuttonDown;

	vec2 m_windowOffset;
	vec2 m_windowSize;

public:

	// �R���X�g���N�^
	Mouse(Window* pWindow)
		: m_pWindow(pWindow)
		, m_lbuttonDown(false)
	{
	}

	// �f�X�g���N�^
	~Mouse()
	{
	}

	void update();
	bool getLButtonDown() {
		return m_lbuttonDown;
	}

	vec2 getRawCursorPos() const;
	vec2 getCursorPos() const;
	void setWindowSize(vec2 offset, vec2 size);

}; // Mouse

