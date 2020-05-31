
#pragma once

#include "cube.h"
#include "enemy.h"

class NormalEnemy : public Enemy
{
private:
	f32 m_veloMax;

public:

	NormalEnemy()
	{
	}

	virtual void onInit(vec2 pos, f32 angle, u32 life, f32 veloMax, f32 length, u32 combo);

	virtual void onUpdate(f32 dt);

	virtual void onRender();

}; // class Enemy

