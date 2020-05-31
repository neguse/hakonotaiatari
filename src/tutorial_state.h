
#pragma once

#include "state.h"

class TutorialState : public IState
{
private:
	u32 m_tick;

public:

	virtual void onInit();
	virtual void onUpdate(f32 dt);
	virtual void onRender();
	virtual void onExit();
	virtual IState* nextState();

	void newEnemy();

}; // class GameState

