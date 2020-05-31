
#pragma once

#include "state.h"
#include "enemy_generator.h"

class GameState : public IState
{
private:
	u32 m_tick;
	EnemyGenerator m_generator;

public:

	virtual void onInit();
	virtual void onUpdate(f32 dt);
	virtual void onRender();
	virtual void onExit();
	virtual IState* nextState();

}; // class GameState

