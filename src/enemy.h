
#pragma once

#include "cube.h"

class Enemy : public Cube
{
	
protected:
	u32 m_tick;

public:

	Enemy()
	{
	}

	virtual void onInit(vec2 pos, f32 angle, u32 life, f32 len, u32 combo);

	virtual void onUpdate(f32 dt) = 0;

	virtual void onRender() = 0;

	virtual void onCollide(Cube* pCube);

	void doFuttobasare( Cube* pCube );

	virtual u32 collStat() const;

	void emitSmog();
	void emitFire(u32 t);

}; // class Enemy

