
#pragma once

#include "cube.h"
#include "enemy.h"

class DashEnemy : public Enemy
{
private:
	f32 m_dashLen;
	f32 m_dashV;
	f32 m_initLen;

public:

	DashEnemy()
	{
	}

	virtual void onInit(vec2 pos, f32 angle, u32 life, f32 dash_v, f32 length, u32 combo);

	virtual void onUpdate(f32 dt);

	virtual void onRender();

}; // class Enemy

