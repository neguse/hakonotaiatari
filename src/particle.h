
#pragma once

class Particle
{

private:
  vec3 m_pos;
  vec3 m_velo;
  vec3 m_acc;
  u32 m_color;
  u32 m_tick;
  u32 m_size;

public:
	
	void onInit(vec3 pos = vec3(), vec3 velo = vec3(), vec3 acc = vec3(), u32 color = 0, u32 tick = 600, u32 size = 2);

	void onUpdate(u32 frames);

	void render();

	bool isDead() const
	{
		return m_tick == 0;
	}

}; // class Particle
