
#include "precompile.h"
#include "enemy.h"

#include "app.h"
#include "player.h"
#include "util.h"

void Enemy::onInit(vec2 pos, f32 angle, u32 life, f32 len, u32 combo )
{
	Cube::onInit(C_TYPE_PLAYER, pos, 0.0f, angle, E_COL_NORMAL, len, combo);
	m_stat = E_ST_APPEAR;
	m_tick = 0;
	m_life = life;
	m_collEnable = false;
}

void Enemy::onCollide(Cube* pCube)
{
	CHECK(pCube->collStat() != C_COL_ST_NONE) << "‚ä‚¤‚ê‚¢ó‘Ô‚Ì‚Æ‚«‚Í“–‚½‚è”»’è‚ª‚È‚¢‚Ì‚ÅonCollide‚É—ˆ‚È‚¢‚Í‚¸";
	CHECK(collStat() != C_COL_ST_NONE) << "‚ä‚¤‚ê‚¢ó‘Ô‚Ì‚Æ‚«‚Í“–‚½‚è”»’è‚ª‚È‚¢‚Ì‚ÅonCollide‚É—ˆ‚È‚¢‚Í‚¸";

	if (pCube->isPlayer()) {
		switch (pCube->collStat()) {
		case C_COL_ST_NORMAL:
			break;
		case C_COL_ST_DASH:
			switch (collStat()) {
			case C_COL_ST_NORMAL:
				// “G:‚Ó‚Â‚¤ ƒvƒŒƒCƒ„[:‚½‚¢‚ ‚½‚è ¨ ‚Ó‚Á‚Æ‚Î‚³‚ê‚é
				doFuttobasare(pCube);
				break;
			}
			break;
		case C_COL_ST_FUTTOBI:
			break;
		}
	} else if (pCube->isEnemy()) {
		if (collStat() == C_COL_ST_NORMAL && pCube->collStat() == C_COL_ST_FUTTOBI) {
			doFuttobasare(pCube);
		} else {
			Cube::onCollide(pCube);
		}
	}
}

u32 Enemy::collStat() const
{
	switch (m_stat) {
	case E_ST_APPEAR:
		return C_COL_ST_NONE;
	case E_ST_MUTEKI:
		return C_COL_ST_NONE;
	case E_ST_NORMAL:
		return C_COL_ST_NORMAL;
	case E_ST_DASH:
		return C_COL_ST_DASH;
	case E_ST_FUTTOBI:
		return C_COL_ST_FUTTOBI;
	case E_ST_FADEOUT:
		return C_COL_ST_NONE;
	case E_ST_DEAD:
		return C_COL_ST_NONE;
	}

	return C_COL_ST_NONE;
}

void Enemy::doFuttobasare( Cube* pCube )
{
	App::setPlayWave(WAVE_HIT4_INDEX);

	m_life--;
	u32 combo = pCube->combo() + m_combo + 1;
	App::addScore(combo);
	setCombo(combo);
	m_stat = E_ST_FUTTOBI;
	m_color = E_COL_FUTTOBI;
	m_tick = 0;
	vec2 dv = m_pos - pCube->pos();
	m_angle = std::atan2(dv.y, dv.x);
	m_collEnable = true;
}

void Enemy::emitSmog()
{
	EmitConeParticle(
		3,
		vec3(pos().x, m_length, pos().y),
		PI - m_angle,	0.4f,
		0.f,			0.4f,
		50.f, 50.f,
		50, 50,
		vec3(0.f, 0.f, 0.f), vec3(40.0f, 20.f, 40.0f),
		0xff404040);

}

void Enemy::emitFire(u32 t)
{
	EmitConeParticle(
		2 + t / 20,
		vec3(pos().x, 0.f, pos().y),
		0.f,		0.2f,
		PI * 0.5f,	0.2f,
		100.f, 100.f,
		50, 50,
		vec3(0.f, -100.f, 0.f), vec3(150.0f, 0.f, 150.0f),
		0xffff0000);
}
