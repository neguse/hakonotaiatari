
#pragma once

#include "state.h"

class TitleState : public IState
{
private:
	u32 m_tick;

public:

	virtual void onInit();
	virtual void onUpdate(f32 dt);
	virtual void onExit();
	virtual void onRender();
	virtual IState* nextState();

}; // class TitleState

