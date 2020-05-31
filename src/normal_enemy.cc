
#include "precompile.h"
#include "normal_enemy.h"

#include "app.h"
#include "player.h"
#include "util.h"

void NormalEnemy::onInit( vec2 pos, f32 angle, u32 life, f32 velo, f32 length, u32 combo )
{
	Enemy::onInit(pos, angle, life, length, combo);
	m_type = C_TYPE_NORMAL_ENEMY;
	m_veloMax = velo;
}

void NormalEnemy::onUpdate(f32 dt)
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
			if (m_tick > E_MUTEKI_F) {
				m_velo = 0.0f;
				m_stat = E_ST_NORMAL;
				m_collEnable = true;
			}
			break;

		case E_ST_NORMAL:
			m_angle += SubRad(dva, m_angle) * 0.02;
			if (m_stat == E_ST_NORMAL) {
				m_velo += (m_veloMax - m_velo) * 0.03;
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
			m_length += -m_length * 0.03;
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

void NormalEnemy::onRender()
{
	if ((m_stat == E_ST_MUTEKI || m_stat == E_ST_APPEAR)
			&& ((m_tick / 2) % 2) == 0) {
	} else {
		Cube::onRender();
	}
}
