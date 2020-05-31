#include "precompile.h"
#include "particle.h"

#include "app.h"
#include "renderer.h"

void Particle::onInit( vec3 pos /*= vec3()*/, vec3 velo /*= vec3()*/, vec3 acc /*= vec3()*/, u32 color /*= 0*/, u32 tick /*= 600*/ , u32 size)
{
	m_pos = pos;
	m_velo = velo;
	m_color = color;
	m_acc = acc;
	m_tick = tick;
	m_size = size;
}

void Particle::onUpdate( u32 frames )
{
	m_pos += m_velo * (DELTA_T * frames);
	m_velo += m_acc * (DELTA_T * frames);
	if (m_pos.y < 0.f) {
		m_pos.y = 0.f;
		m_velo.y *= -0.8f;
	}
	if (m_tick > 0) {
		m_tick = Max<i32>(m_tick - frames, 0);
	}
}

void Particle::render()
{
	App::pRenderer->renderDot(m_pos, m_color, m_size);
}
