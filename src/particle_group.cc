#include "precompile.h"
#include "particle_group.h"

#include "app.h"
#include "renderer.h"


ParticleGroup::ParticleGroup()
{

}

ParticleGroup::~ParticleGroup()
{

}

Particle* ParticleGroup::pop()
{
	if (m_dead.empty()) {
		return new Particle();
	} else {
		Particle* pRet = *m_dead.begin();
		m_dead.pop_front();
		return pRet;
	}
}

void ParticleGroup::push( Particle* pParticle )
{
	m_living.push_back(pParticle);
}

void ParticleGroup::onUpdate( u32 frames )
{
	for (std::list<Particle*>::iterator it = m_living.begin(); it != m_living.end(); it++) {
		(*it)->onUpdate(frames);
	}
	std::list<Particle*>::iterator it = m_living.begin();
	for (; it != m_living.end();) {
		if ((*it)->isDead() ) {
			std::list<Particle*>::iterator next = it;
			next++;
			m_dead.push_back(*it);
			m_living.erase(it);
			it = next;
		} else {
			it++;
		}
	}
}

void ParticleGroup::render()
{
	App::pRenderer->setModel(mat4());
	App::pRenderer->updateMVP();
	for (std::list<Particle*>::iterator it = m_living.begin(); it != m_living.end(); it++) {
		(*it)->render();
	}
}
