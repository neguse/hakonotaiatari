
#include "precompile.h"
#include "dash_enemy.h"

#include "app.h"
#include "player.h"
#include "util.h"

void DashEnemy::onInit( vec2 pos, f32 angle, u32 life, f32 dash_v, f32 length, u32 combo )
{
	Enemy::onInit(pos, angle, life, length, combo + 50);
	m_initLen = length;
	m_type = C_TYPE_DASH_ENEMY;
	m_dashV = dash_v;
}

void DashEnemy::onUpdate(f32 dt)
{
	m_tick++;

	vec2 dv = App::pPlayer->pos() - pos();
	f32 dva = std::atan2(dv.y, dv.x);
	CHECK(!_isnan(dv.x)) << "IS NOT NAN!";

	u32 prev_stat = m_stat;
	switch (m_stat) {
		case E_ST_APPEAR:
			if (m_tick > E_APPEAR_F) {
				m_stat = E_ST_NORMAL;
				m_collEnable = true;
			}
			break;

		case E_ST_MUTEKI:
			m_length += (m_initLen - m_length) * 0.05;

			if (m_tick > E_MUTEKI_F) {
				m_velo = 0.0f;
				m_stat = E_ST_NORMAL;
				m_collEnable = true;
			}
			break;

		case E_ST_NORMAL:
			{
				m_length += (m_initLen - m_length) * 0.05;

				f32 dav = (m_tick < 45) ? 0.01f : 0.09f;
				m_angle += SubRad(dva, m_angle) * dav;

				if (m_tick > 45 && fabs(SubRad(dva, m_angle)) < 0.1f) {
					m_stat = E_ST_DASH;
					m_velo = E_DASH_V;
					m_color = E_COL_DASH;
					m_collEnable = true;
					m_dashLen = m_length + E_DASH_LEN;
					App::setPlayWave(WAVE_SUBERIE_INDEX);
				} else {
					m_velo += (E_NORMAL_V - m_velo) * 0.03;
				}
			}
			break;

		case E_ST_DASH:
			m_length += (m_dashLen - m_length) * 0.1;

			if (m_tick > E_DASH_F) {
				m_stat = E_ST_NORMAL;
				m_color = E_COL_NORMAL;
				m_collEnable = true;
			}
			{
				vec2 p = pos();
				p -= Pcs(m_length, m_angle);
				EmitConeParticle(
					1,
					vec3(p.x, m_length, p.y),
					PI + m_angle, PI,
					PI * 0.25, PI,
					300.f, 70.f,
					50, 10,
					vec3(0.f, -100.f, 0.f), vec3(5.0f, 0.f, 5.0f),
					E_COL_DASH_PARTICLE);
			}
			break;

		case E_ST_FUTTOBI:
			m_velo = E_FUTTOBI_V;
			emitSmog();

			if (isOutOfArea()) {
				m_velo = 0;
				if (m_life > 0) {
					m_stat = E_ST_MUTEKI;
					m_color = E_COL_NORMAL;
					m_collEnable = false;
					App::setPlayWave(WAVE_REVIRTH_INDEX, true, false);
				} else {
					m_stat = E_ST_FADEOUT;
					m_collEnable = false;
					App::setPlayWave(WAVE_FIRE_INDEX, true, false);
				}
			}
			break;

		case E_ST_FADEOUT:
			m_length += -m_length * 0.05;
			emitFire(Max<i32>((E_FADEOUT_F - m_tick), 0));
			if (m_tick > E_FADEOUT_F) {
				m_stat = E_ST_DEAD;
			}
			break;

	}

	if (m_stat != prev_stat) {
		m_tick = 0;
	}
	CHECK(!_isnan(m_pos.x)) << "IS NOT NAN!";

	Cube::clampPosition();
	Cube::onUpdate(dt);

	CHECK(!_isnan(m_pos.x)) << "IS NOT NAN!";

}

void DashEnemy::onRender()
{
	if ((m_stat == E_ST_MUTEKI || m_stat == E_ST_APPEAR)
			&& ((m_tick / 2) % 2) == 0) {
	} else {
		Cube::onRender();

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

		quat ModelRot(vec3(0.0f, (timeGetTime() % 500) * 2.0f * PI / 500.f, 0.0f));
		mat4 ModelScale = glm::scale(
			mat4(1.0f),
			vec3(m_length * (1.0 / SQRT2)));
		mat4 ModelTrans = glm::translate(
			mat4(1.0f),
			vec3(m_pos.x, 0.f, m_pos.y));
		mat4 model = ModelTrans * ModelScale * glm::mat4_cast(ModelRot);

		App::pRenderer->setModel(model);
		App::pRenderer->updateMVP();
		App::pRenderer->renderWire(
			verts, verts_count,
			indices, indices_count,
			m_color);

	}
}
