
#pragma once

class Cube
{

protected:
	u32 m_type;
	vec2 m_pos;
	f32 m_velo;
	f32 m_angle;
	u32 m_color;
	f32 m_length;
	vec2 m_force;
	u32 m_stat;
	i32 m_life;
	bool m_collEnable;
	u32 m_combo;

public:

	Cube()
	{
		onInit(0, vec2(), 0, 0, 0, 0, 0);
	}

	virtual void onInit(u32 type, vec2 pos, f32 velo, f32 angle, u32 color, f32 length, u32 combo);

	virtual void onUpdate(f32 dt);

	virtual void onCollide(Cube* pCube);

	bool isOutOfArea() const;

	void clampPosition();

	virtual void onRender();

	vec2 pos() const
	{
		return m_pos;
	}

	f32 length() const
	{
		return m_length;
	}

	f32 angle() const
	{
		return m_angle;
	}

	u32 stat() const
	{
		return m_stat;
	}

	virtual u32 collStat() const = 0;

	i32 life() const
	{
		return m_life;
	}
	void setLife(i32 life)
	{
		m_life = life;
	}

	u32 combo() const
	{
		return m_combo;
	}
	void setCombo(u32 combo)
	{
		m_combo = combo;
	}
	bool isCollEnable() const
	{
		return m_collEnable;
	}

	u32 type() const
	{
		return m_type;
	}

	static bool IsCubeCollide(Cube* pCube1, Cube* pCube2);

	bool isPlayer() const
	{
		return m_type == C_TYPE_PLAYER;
	}

	bool isEnemy() const
	{
		return (m_type | C_TYPE_ENEMY_MASK);
	}

}; // class Cube

