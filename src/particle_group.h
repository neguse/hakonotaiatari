
#pragma once

#include "particle.h"

class ParticleGroup
{

private:

  std::list<Particle*> m_living;
  std::list<Particle*> m_dead;

public:

	ParticleGroup();
	virtual ~ParticleGroup();

	Particle* pop();
	void push(Particle* pParticle);
	void onUpdate(u32 frames);
	void render();

	size_t getCount() const
	{
		return m_living.size();
	}

}; // class ParticleGroup
