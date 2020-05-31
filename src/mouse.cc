
#include "precompile.h"
#include "mouse.h"
#include "window.h"

static const unsigned int MOUSE_BUTTON_VK[Mouse::MOUSE_BUTTON_MAX] = {
	VK_LBUTTON,
	VK_RBUTTON,
	VK_MBUTTON,
}; // class Mouse

void Mouse::update()
{
	m_lbuttonDown = m_pWindow->getLButtonDownCount() > 0;
	m_pWindow->clearLButtonCount();
}

vec2 Mouse::getRawCursorPos() const
{
	POINT p;
	::GetCursorPos(&p);
	::ScreenToClient(m_pWindow->getHandle(), &p);
	return vec2(p.x, p.y);
}

vec2 Mouse::getCursorPos() const
{
	vec2 p = getRawCursorPos() - m_windowOffset;
	vec2 windowSize = m_windowSize;
	if ((fabs(0.f - windowSize.x) < 0.01f)
		&& (fabs(0.f - windowSize.y) < 0.01f))
	{
		return vec2();
	}

	vec2 ret = (vec2(p.x, -p.y) + windowSize * vec2(-0.5f, 0.5f)) * 2.0f / windowSize;

	return ret;
}

void Mouse::setWindowSize(vec2 offset, vec2 size )
{
	m_windowOffset = offset;
	m_windowSize = size;
}

