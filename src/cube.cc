
#include "precompile.h"
#include "cube.h"

#include "app.h"
#include "renderer.h"
#include "particle.h"
#include "particle_group.h"

void Cube::onInit(u32 type, vec2 pos, f32 velo, f32 angle, u32 color, f32 length, u32 combo)
{
	m_type = type;
	m_pos = pos;
	m_velo = velo;
	m_angle = angle;
	m_color = color;
	m_length = length;
	m_combo = combo;
}

void Cube::onUpdate(f32 dt)
{
	CHECK(!_isnan(m_force.x)) << "IS NOT NAN!";
	CHECK(!_isnan(m_angle)) << "IS NOT NAN!";
	CHECK(!_isnan(dt)) << "IS NOT NAN!";

	m_pos += dt * Pcs(m_velo, m_angle);
	m_pos += m_force;
	m_force = vec2();

	CHECK(!_isnan(m_pos.x)) << "IS NOT NAN!";

}

void Cube::onCollide(Cube* pCube)
{
	vec2 d = pCube->pos() - m_pos;
	f32 l = (m_length + pCube->length()) * SQRT2;
	f32 dl = glm::length(d);
	m_force -= d * 0.5f * ((l - dl) / l);

	EmitConeParticle(
		1,
		vec3(m_pos.x, 0.f, m_pos.y),
		0.f, PI * 2.0f,
		PI * 0.5f, PI * 0.25f,
		1000.f, 1000.f,
		60, 0,
		vec3(0.f, -1500.f, 0.f), vec3(5.0f, 0.f, 5.0f),
		0xffff0000);
}

bool Cube::isOutOfArea() const
{
	return (m_pos.x - m_length) < -FIELD_Lf || FIELD_Lf < (m_pos.x + m_length)
		|| (m_pos.y - m_length) < -FIELD_Lf || FIELD_Lf < (m_pos.y + m_length);
}

void Cube::clampPosition()
{
	m_pos.x = Clamp(m_pos.x, -FIELD_Lf + m_length, FIELD_Lf - m_length);
	m_pos.y = Clamp(m_pos.y, -FIELD_Lf + m_length, FIELD_Lf - m_length);
}

void Cube::onRender()
{
		static const u32 verts_count = 8;
		static const u32 indices_count = 24;
		static const vec3 verts[verts_count] = {
			vec3(-1.0f,  0.0f, -1.0f),
			vec3( 1.0f,  0.0f, -1.0f),
			vec3( 1.0f,  2.0f, -1.0f),
			vec3(-1.0f,  2.0f, -1.0f),

			vec3(-1.0f,  0.0f,  1.0f),
			vec3( 1.0f,  0.0f,  1.0f),
			vec3( 1.0f,  2.0f,  1.0f),
			vec3(-1.0f,  2.0f,  1.0f),
		};

		u32 indices[indices_count] = {
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			0, 4,
			1, 5,
			2, 6,
			3, 7,
		};

		quat ModelRot(vec3(0.0f, -m_angle, 0.0f));
		mat4 ModelScale = glm::scale(
				mat4(1.0f),
				vec3(m_length));
		mat4 ModelTrans = glm::translate(
				mat4(1.0f),
				vec3(m_pos.x, 0.0f, m_pos.y));
		mat4 model = ModelTrans * ModelScale * glm::mat4_cast(ModelRot);

		App::pRenderer->setModel(model);
		App::pRenderer->updateMVP();
		App::pRenderer->renderWire(
				verts, verts_count,
				indices, indices_count,
				m_color);
}

bool Cube::IsCubeCollide(Cube* pCube1, Cube* pCube2)
{
	return IsCollideSquare(
		pCube1->pos(), pCube1->length(), pCube1->angle(),
		pCube2->pos(), pCube2->length(), pCube2->angle());
}

