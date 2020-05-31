
#pragma once

#include "cube.h"

class Player : public Cube
{

private:
	u32 m_tick;
	f32 m_dashLen; // ‚½‚¢‚ ‚½‚èŒã‚Ì‘å‚«‚³
	u32 m_dashF; // ‚½‚¢‚ ‚½‚èƒtƒŒ[ƒ€”
	u32 m_power;

public:

	Player()
		: Cube()
	{
	}

	virtual void onInit();

	virtual void onUpdate(f32 dt);

	virtual void onRender();

	virtual void onCollide(Cube* pCube);

	virtual u32 collStat() const;

	u32 calcDashF() const;
	void addPower(i32 d);

}; // class Player

