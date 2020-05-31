
#include "precompile.h"
#include "player.h"

#include "app.h"
#include "mouse.h"
#include "camera.h"
#include "renderer.h"
#include "particle.h"
#include "particle_group.h"

void Player::onInit()
{
	Cube::onInit(C_TYPE_PLAYER, vec2(), 0.0f, 0.0f, P_COL_NORMAL, P_LEN, 0);
	m_stat = P_ST_MUTEKI;
	m_tick = 0;
	m_power = 0;
	m_life = P_LIFE_INIT;
	m_collEnable = false;
}

void Player::onUpdate(f32 dt)
{
	m_tick++;
	vec2 mpos = App::mousePos();
	vec2 dv = mpos - m_pos;
	f32 dva = std::atan2(dv.y, dv.x);
	f32 dvl = glm::length(dv);
	u32 prev_stat = m_stat;

	switch (m_stat) {
		case P_ST_MUTEKI:
			if (m_tick > P_MUTEKI_F) {
				m_stat = P_ST_NORMAL;
				m_collEnable = true;
			}
			// 意図的なfall through. 無敵中も動かせるようにするため

		case P_ST_NORMAL:
			m_length += (P_LEN - m_length) * 0.01;
			m_angle = dva;
			m_velo = glm::min(dvl * P_VR, P_VL);
			addPower(1);
			if (m_power == P_DASH_MIN_POW + 1) {
				App::setPlayWave(WAVE_POWERFULL_INDEX, true, false);
			}
			if (P_DASH_MIN_POW < m_power && App::pMouse->getLButtonDown()) {
				m_stat = P_ST_DASH;
				m_velo = P_DASH_V;
				m_color = P_COL_DASH;
				m_collEnable = true;
				m_dashLen = m_length + P_DASH_LEN;
				m_dashF = calcDashF();
				addPower(-P_DASH_USE_POW);
				App::setPlayWave(WAVE_SUBERI_INDEX);
			}
			break;

		case P_ST_DASH:
			m_length += (m_dashLen - m_length) * 0.1;
			m_velo = P_DASH_V;
			if (m_tick > m_dashF) {
				m_stat = P_ST_NORMAL;
				m_color = P_COL_NORMAL;
				m_collEnable = true;
				if (calcDashF() > 0) {
					App::setPlayWave(WAVE_POWERFULL_INDEX, true, false);
				}
			}
			{
				vec2 p = pos();
				p -= Pcs(m_length, m_angle);
				EmitConeParticle(
					5,
					vec3(p.x, m_length, p.y),
					PI + m_angle, PI,
					PI * 0.25, PI,
					300.f, 70.f,
					50, 10,
					vec3(0.f, -100.f, 0.f), vec3(5.0f, 0.f, 5.0f),
					P_COL_DASH_PARTICLE);
			}
			break;

		case P_ST_FUTTOBI: // 意図的なfall through
		case P_ST_FUTTOBI_NOMUTEKI:
			m_velo = P_FUTTOBI_V;

			if (m_tick > P_FUTTOBI_F) {
				if (m_life > 0) {
					if (m_stat == P_ST_FUTTOBI) {
						m_stat = P_ST_MUTEKI;
						m_tick = 0;
						m_collEnable = false;
					} else {
						CHECK(m_stat == P_ST_FUTTOBI_NOMUTEKI);
						// m_stat = P_ST_NORMAL;
						// m_collEnable = true;
						m_stat = P_ST_MUTEKI;
						m_tick = P_MUTEKI_F - P_NOMUTEKI_F;
						m_collEnable = false;
					}
					m_color = P_COL_NORMAL;
				} else {
					App::setPlayWave(WAVE_FALL_INDEX, true, false);
					m_stat = P_ST_FADEOUT;
					App::pCamera->setEnableRotate(true);
					m_collEnable = false;
				}
			}
			break;

		case P_ST_FADEOUT:
		case P_ST_DEAD:
			m_length += -m_length * 0.03;
			if (m_tick > P_FADEOUT_F && m_stat == P_ST_FADEOUT) {
				App::saveRecord();
				m_stat = P_ST_DEAD;
				swprintf(App::noticeBuffer, L"game over");
			}
			break;
	}

	if (m_stat == P_ST_DASH) {
		App::pCamera->setBehind(CAM_BEHIND_HIGH_DASH, CAM_BEHIND_BACK_DASH);
	} else if (m_stat == P_ST_DEAD) {
		App::pCamera->setBehind(CAM_BEHIND_HIGH_DEAD, CAM_BEHIND_BACK_DEAD);
	} else {
		App::pCamera->setBehind(CAM_BEHIND_HIGH, CAM_BEHIND_BACK);
	}

	if (prev_stat != m_stat && m_stat != P_ST_MUTEKI) { // 無敵時間の調整のため、無敵の場合はここではクリアしない
		m_tick = 0;
	}

	Cube::onUpdate(dt);

	// ちょっとだけカーソル位置の方を向いておく
	if (m_stat != P_ST_DEAD) {
		vec2 la = m_pos + Pcs(dvl * 0.05f, dva);
		App::pCamera->setLookat(vec3(Clamp(la.x, -FIELD_Lf, FIELD_Lf), 0.0f, Clamp(la.y, -FIELD_Lf, FIELD_Lf)));
	} else {
		App::pCamera->setLookat(vec3(m_pos.x, 0.f, m_pos.y));
	}
}

void Player::onRender()
{
	if (m_stat == P_ST_MUTEKI && (m_tick / 4) % 4 == 0) {
	} else {
		Cube::onRender();
	}

	// 進行方向に線を引く(体当たり時の移動距離をわかりやすくするため)
	if (m_stat == P_ST_MUTEKI || m_stat == P_ST_NORMAL || m_stat == P_ST_DASH) {

		bool canDash = calcDashF() > 0;
		u32 color = canDash ? P_COL_DASH_GUIDE : P_COL_DASH_GUIDE_DISABLE;
		u32 frame = (m_stat == P_ST_DASH) ? (m_dashF - m_tick) : Max<i32>(calcDashF(), P_POW_GETA);
		f32 length = frame * P_DASH_V * DELTA_T;

		vec3 p1 = vec3(m_pos.x, 0.0f, m_pos.y);
		vec2 ps = Pcs(length, m_angle);
		vec3 p2 = p1 + vec3(ps.x, 0.0f, ps.y);

		mat4 model = mat4(1.0f);
		App::pRenderer->setModel(model);
		App::pRenderer->updateMVP();
		App::pRenderer->renderLine(p1, p2, color);
	}

}

void Player::onCollide(Cube* pCube)
{
	CHECK(pCube->collStat() != C_COL_ST_NONE) << "ゆうれい状態のときは当たり判定がないのでonCollideに来ないはず";
	CHECK(collStat() != C_COL_ST_NONE) << "ゆうれい状態のときは当たり判定がないのでonCollideに来ないはず";
	
	vec2 dv = (m_pos - pCube->pos());
	
	switch (collStat()) {
	case C_COL_ST_NORMAL:

		if (pCube->collStat() == C_COL_ST_NORMAL || pCube->collStat() == C_COL_ST_DASH) {
			App::setPlayWave(WAVE_HIT1_INDEX);

			EmitConeParticle(1000, vec3(m_pos.x, 0.f, m_pos.y),
				0.f, PI * 2.0f,
				0.f, 0.2f,
				500.f, 1000.f,
				100, 20,
				vec3(), vec3(),
				0xffa00000);
			m_angle = std::atan2(dv.y, dv.x);

			m_life--;
			m_stat = P_ST_FUTTOBI;
			m_color = P_COL_FUTTOBI;
			m_tick = 0;
			m_collEnable = true;
			addPower(P_POW_HIT_BONUS);
		}

		break;

	case C_COL_ST_DASH:
		if (pCube->collStat() == C_COL_ST_DASH) {
			// 反発する
			Cube::onCollide(pCube); // とりあえずめり込みを解除
			m_angle = std::atan2(dv.y, dv.x); // 角度を敵と反対方向に
			m_stat = P_ST_FUTTOBI_NOMUTEKI;
			App::setPlayWave(WAVE_NAGU_INDEX, true, false);
		}
		break;
	}
}

u32 Player::collStat() const
{
	switch (m_stat) {
	case P_ST_MUTEKI:
		return C_COL_ST_NONE;
	case P_ST_NORMAL:
		return C_COL_ST_NORMAL;
	case P_ST_DASH:
		return C_COL_ST_DASH;
	case P_ST_FUTTOBI:
		return C_COL_ST_FUTTOBI;
	case P_ST_FUTTOBI_NOMUTEKI:
		return C_COL_ST_FUTTOBI;
	case P_ST_FADEOUT:
		return C_COL_ST_NONE;
	case P_ST_DEAD:
		return C_COL_ST_NONE;
	}
	return C_COL_ST_NONE;
}

u32 Player::calcDashF() const
{
	if (m_power < P_DASH_MIN_POW) {
		return 0;
	} else {
		return P_POW_GETA + P_POW_COEFF * (m_power - P_DASH_MIN_POW);
	}
}

void Player::addPower( i32 d )
{
	m_power = Clamp<i32>(m_power + d, 0, P_POW_MAX);
}
