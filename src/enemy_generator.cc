
#include "precompile.h"
#include "enemy_generator.h"

#include "app.h"

#include "normal_enemy.h"
#include "dash_enemy.h"

namespace {

	enum EvalType {
		EVAL_UPPER = 2, // éûä‘Ç©Ç©ÇËÇ∑Ç¨
		EVAL_CENTER = 1, // Ç”Ç¬Å[
		EVAL_LOWER = 0, // Ç§Ç‹Ç¢Ç§Ç‹Ç∑Ç¨ÇÈ
	};

	enum LevelId {
		LEVEL_POP0,
		LEVEL_POP1,
		LEVEL_POP2,
		LEVEL_POP3,
		LEVEL_POP8,
		LEVEL_POP8_HARD,
		LEVEL_TIME1,
		LEVEL_TIME2,
		LEVEL_TIME3,
		LEVEL_WARAWARA,
		LEVEL_WARAWARA_HARD,
		LEVEL_EXPLODE,
		LEVEL_EXPLODE_HARD,
		LEVEL_ROUND0,
		LEVEL_ROUND1,
		LEVEL_ROUND2,
		LEVEL_ROUND8,
		LEVEL_SPEED1,
		LEVEL_SPEED2,
		LEVEL_SPEED8,
		LEVEL_BIG1,
		LEVEL_BIG2,
		LEVEL_BIG8,
		LEVEL_BARA1,
		LEVEL_BARA2,
		LEVEL_BARA8,
		LEVEL_BOSS1,
		LEVEL_BOSS2,
		LEVEL_BOSS2_OTOMO,
		LEVEL_BOSS3,
		LEVEL_DEMO,
	};

	class SleepCommand : public IEnemyGeneratorCommand
	{
	private:
		u32 m_tick;
	public:
		SleepCommand(u32 tick)
			: m_tick(tick)
		{

		}

		virtual bool done() const
		{
			return m_tick == 0;
		}

		virtual void step(f32 dt)
		{
			if (m_tick > 0) {
				m_tick--;
			}
		}
	}; // SleepCommand

	class NormalEnemyAtPointCommand : public IEnemyGeneratorCommand
	{
	private:
		u32 m_restCount;
		u32 m_life;
		vec2 m_pos;
		f32 m_veloMax;
		f32 m_length;

	public:

		NormalEnemyAtPointCommand(vec2 pos, u32 count, u32 life, f32 veloMax = E_NORMAL_V, f32 length = E_LEN)
			: m_pos(pos), m_restCount(count), m_life(life), m_veloMax(veloMax), m_length(length)
		{
		}

		virtual bool done() const
		{
			return m_restCount == 0;
		}

		virtual void step( f32 dt ) 
		{
			if (!done()) {
				f32 angle = RandRange<f32>(0.f, 2.0f * PI);
				NormalEnemy* pEnemy = new NormalEnemy();
				pEnemy->onInit(m_pos, angle, m_life, m_veloMax, m_length, 0);
				App::enemies.push_back(pEnemy);
				m_restCount = Max<i32>(m_restCount - 1, 0);
			}
		}

	}; // NormalEnemyAtPointCommand

	class NormalEnemyAtNearPlayerCommand : public IEnemyGeneratorCommand
	{
	private:
		f32 m_distance;
		u32 m_restCount;
		u32 m_life;
		f32 m_veloMax;
		f32 m_length;

	public:

		NormalEnemyAtNearPlayerCommand(f32 distance, u32 count, u32 life, f32 veloMax = E_NORMAL_V, f32 length = E_LEN)
			: m_distance(distance), m_restCount(count), m_life(life), m_veloMax(veloMax), m_length(length)
		{
		}

		virtual bool done() const
		{
			return m_restCount == 0;
		}

		virtual void step( f32 dt ) 
		{
			if (!done()) {
				f32 angle = RandRange<f32>(0.f, 2.0f * PI);
				vec2 pos = App::pPlayer->pos() + Pcs(m_distance, angle);
				NormalEnemy* pEnemy = new NormalEnemy();
				pEnemy->onInit(pos, angle, m_life, m_veloMax, m_length, 0);
				App::enemies.push_back(pEnemy);
				m_restCount = Max<i32>(m_restCount - 1, 0);
			}
		}

	}; // NormalEnemyAtNearPlayerCommand

	class DashEnemyAtPointCommand : public IEnemyGeneratorCommand
	{
	private:
		u32 m_restCount;
		u32 m_life;
		vec2 m_pos;
		f32 m_length;

	public:

		DashEnemyAtPointCommand(vec2 pos, u32 count, u32 life, f32 length = E_LEN)
			: m_pos(pos), m_restCount(count), m_life(life), m_length(length)
		{
		}

		virtual bool done() const
		{
			return m_restCount == 0;
		}

		virtual void step( f32 dt ) 
		{
			if (!done()) {
				f32 angle = RandRange<f32>(0.f, 2.0f * PI);
				DashEnemy* pEnemy = new DashEnemy();
				pEnemy->onInit(m_pos, angle, m_life, E_DASH_V, m_length, 0);
				App::enemies.push_back(pEnemy);
				m_restCount = Max<i32>(m_restCount - 1, 0);
			}
		}

	}; // DashEnemyAtPointCommand

}

EnemyGenerator::EnemyGenerator()
	: m_level(0)
	, m_upperCount(0)
	, m_centerCount(0)
	, m_lowerCount(0)
{
	nextLevel();
}

EnemyGenerator::~EnemyGenerator()
{
	while (!m_commands.empty()) {
		IEnemyGeneratorCommand* pCommand = *m_commands.begin();
		delete pCommand;
		m_commands.pop_front();
	}
}

void EnemyGenerator::update( f32 dt )
{
	m_levelTick++;

	std::map<u32, u32> enemy_count;
	for (auto it = App::enemies.begin(); it != App::enemies.end(); it++) {
		if ((*it)->isEnemy()) {
			enemy_count[(*it)->type()]++;
		}
	}

	if (m_commands.empty() && enemy_count.empty()) {
		nextLevel();
	} else if (!m_commands.empty()) {
		IEnemyGeneratorCommand* pCommand = *m_commands.begin();

		pCommand->step(dt);
		if (pCommand->done()) {
			m_commands.pop_front();
			delete pCommand;
		}
	}
}

void EnemyGenerator::onRender()
{
	// TODO: í≤êÆóp
	/*
	wchar_t buf[1024];
	swprintf(buf, L"level: %2d, eval:%2.2f / %2.2f", m_level, m_eval, m_evalMean);
	App::renderText(vec2(1, 3), 0xffffffff, buf);
	swprintf(buf, L"ñ⁄ïW: %2.2f~%2.2f(ïb)", m_levelLowerTick / (f32)FPS, m_levelUpperTick / (f32)FPS);
	App::renderText(vec2(1, 2), 0xffffffff, buf);
	swprintf(buf, L"ç°: %2.2f(ïb)", m_levelTick / (f32)FPS);
	App::renderText(vec2(1, 1), 0xffffffff, buf);
	*/
}

void EnemyGenerator::nextLevel()
{
	m_level++;

	LOG(INFO) << "cleared. tick:" << m_levelTick;
	LOG(INFO) << "low:" << m_levelLowerTick << " up:" << m_levelUpperTick;

	// ç°âÒÇÃÉNÉäÉAéûä‘ï]âø
	u32 eval = EVAL_CENTER;
	
	if (m_level > 1) {
		if (m_levelTick < m_levelLowerTick) {
			m_lowerCount++;
			eval = EVAL_LOWER;
		} else if (m_levelLowerTick < m_levelUpperTick) {
			m_centerCount++;
			eval = EVAL_CENTER;
		} else {
			m_upperCount++;
			eval = EVAL_UPPER;
		}
		m_evalMean = (m_lowerCount * EVAL_LOWER + m_centerCount * EVAL_CENTER + m_upperCount * EVAL_UPPER) / (m_level - 1);
		m_eval = (m_evalMean + (f32)eval) / 2.f;
	} else {
		m_eval = 1.0f;
		m_evalMean = 1.0f;
	}

	m_levelLowerTick = m_levelUpperTick = m_levelTick = 0;

	u32 id = 0;
	if (E_GEN_USE_LEVEL_FIX) {
		id = E_GEN_FIX_LEVEL_ID;
	} else {
		id = getLevelId(m_level, m_eval);
	}

	if ((m_level % 4) == 1) {
		// èââÒ or É{ÉXå„
		// ãxåeÇÇ¢ÇÍÇÈ
		u32 wait = FPS * 3.5;
		m_commands.push_back(new SleepCommand(wait));
		m_levelLowerTick += wait;
		m_levelUpperTick += wait;
	}

	createLevel(id);

	// point: ï]âøíl 0Å`2(0:Ç§Ç‹Ç¢ 2:Ç÷ÇΩ)
	// level: êiçsÉåÉxÉã
	
}

u32 EnemyGenerator::getLevelId( u32 level, f32 eval )
{
	std::vector<u32> kouho;
	switch (level % 4) {
		case 1:
			if (level <= 4) {
				kouho.push_back(LEVEL_POP0);
			} else if (level <= 8) {
				kouho.push_back(LEVEL_POP1);
				kouho.push_back(LEVEL_POP2);
				if (eval < 0.5f) {
					kouho.push_back(LEVEL_POP3);
				}
			} else {
				kouho.push_back(LEVEL_POP8);
				if (eval < 0.5f) {
					kouho.push_back(LEVEL_POP8_HARD);
				}
			}
			break;
		case 2:
			if (level <= 4) {
				kouho.push_back(LEVEL_TIME1);
				kouho.push_back(LEVEL_TIME2);
			} else if (level <= 8) {
				kouho.push_back(LEVEL_TIME3);
				kouho.push_back(LEVEL_ROUND0);
				kouho.push_back(LEVEL_SPEED1);
				if (eval < 0.5f) {
					kouho.push_back(LEVEL_ROUND1);
				}
			} else {
				kouho.push_back(LEVEL_ROUND2);
				kouho.push_back(LEVEL_SPEED2);
				if (eval < 0.5f) {
					kouho.push_back(LEVEL_ROUND8);
					kouho.push_back(LEVEL_SPEED8);
				}
			}
			break;
		case 3:
			if (level <= 4) {
				if (0.5f < eval) {
					kouho.push_back(LEVEL_BIG1);
				} else {
					kouho.push_back(LEVEL_BIG2);
					kouho.push_back(LEVEL_BARA1);
				}
			} else if (level <= 8) {
				kouho.push_back(LEVEL_BARA1);
				kouho.push_back(LEVEL_BIG2);
				if (eval < 0.5f) {
					kouho.push_back(LEVEL_WARAWARA);
					kouho.push_back(LEVEL_EXPLODE);
				}
			} else {
				kouho.push_back(LEVEL_WARAWARA);
				kouho.push_back(LEVEL_EXPLODE);
				kouho.push_back(LEVEL_BIG8);
				kouho.push_back(LEVEL_BARA8);
				if (eval < 0.5f) {
					kouho.push_back(LEVEL_WARAWARA_HARD);
					kouho.push_back(LEVEL_EXPLODE_HARD);
				}
			}
			break;

		case 0:
			if (level <= 4) {
				kouho.push_back(LEVEL_BOSS1);
			} else if (level <= 8) {
				kouho.push_back(LEVEL_BOSS2);
			} else {
				kouho.push_back(LEVEL_BOSS2_OTOMO);
				kouho.push_back(LEVEL_BOSS3);
			}
			break;
	}

	LOG(INFO) << "kouho:";
	for (int i = 0; i < kouho.size(); i++) {
		LOG(INFO) << "\t" << kouho[i];
	}

	u32 id = RandVector<u32>(kouho, LEVEL_POP1);

	LOG(INFO) << "selected " << id;

	return id;
}

void EnemyGenerator::createLevel( u32 id )
{
	switch (id) {
	case LEVEL_POP0:
		{
			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 1, 1));

			m_levelLowerTick += FPS * 5.5f;
			m_levelUpperTick += FPS * 13.5f;
		}
		break;
	case LEVEL_POP1:
		{
			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 1, 1));

			for (int i = 0; i < 1; i++) {
				m_commands.push_back(
					new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 5, 1));

				m_commands.push_back(
					new SleepCommand(20));

			}

			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 3, 1));

			m_levelLowerTick += FPS * 8.5f;
			m_levelUpperTick += FPS * 13.5f;
		}
		break;
	case LEVEL_POP2:
		{
			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 1, 1));

			for (int i = 0; i < 2; i++) {
				m_commands.push_back(
					new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 5, 1));

				m_commands.push_back(
					new SleepCommand(20));

			}

			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 3, 1));

			m_levelLowerTick += FPS * 10.0;
			m_levelUpperTick += FPS * 15.5;
		}
		break;
	case LEVEL_POP3:
		{
			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 1, 1));

			for (int i = 0; i < 3; i++) {
				m_commands.push_back(
					new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 5, 1));

				m_commands.push_back(
					new SleepCommand(20));

			}

			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 3, 1));

			m_levelLowerTick += FPS * 10.0;
			m_levelUpperTick += FPS * 15.5;
		}
		break;

	case LEVEL_POP8:
		{
			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 1, 1));

			for (int i = 0; i < 8; i++) {
				m_commands.push_back(
					new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 5, 1));

				m_commands.push_back(
					new SleepCommand(15));

			}

			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 3, 1));

			m_levelLowerTick += FPS * 16.0;
			m_levelUpperTick += FPS * 22.5;
		}
		break;

	case LEVEL_POP8_HARD:
		{
			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 1, 2));

			for (int i = 0; i < 8; i++) {
				m_commands.push_back(
					new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 5, 2));

				m_commands.push_back(
					new SleepCommand(15));

			}

			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 3, 2));

			m_levelLowerTick += FPS * 18.0;
			m_levelUpperTick += FPS * 25.5;
		}
		break;

	case LEVEL_TIME1:
		{
			for (int i = 0; i < 3; i++) {
				m_commands.push_back(
					new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 1, 1));
				m_commands.push_back(new SleepCommand(120));
			}

			m_levelLowerTick += FPS * 12.0;
			m_levelUpperTick += FPS * 20.0;
		}
		break;
	
	case LEVEL_TIME2:
			for (int i = 0; i < 5; i++) {
				m_commands.push_back(
					new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 1, 1));
				m_commands.push_back(new SleepCommand(100));
			}

			m_levelLowerTick += FPS * 13.0;
			m_levelUpperTick += FPS * 20.0;
		break;

	case LEVEL_TIME3:
			for (int i = 0; i < 5; i++) {
				m_commands.push_back(
					new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 2, 1));
				m_commands.push_back(new SleepCommand(100));
			}

			m_levelLowerTick += FPS * 16.0;
			m_levelUpperTick += FPS * 25.0;
		break;

	case LEVEL_WARAWARA:
		{
			for (int i = 0; i < 20; i++) {
				m_commands.push_back(new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf * 2.f), RandRangeMid(0.f, FIELD_Lf * 2.f)), 3, 1));
				m_commands.push_back(new SleepCommand(2));
			}

			m_levelLowerTick += FPS * 12.0f;
			m_levelUpperTick += FPS * 18.0f;
		}
		break;

	case LEVEL_WARAWARA_HARD:
		{
			for (int i = 0; i < 20; i++) {
				m_commands.push_back(new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf * 2.f), RandRangeMid(0.f, FIELD_Lf * 2.f)), 3, 2));
				m_commands.push_back(new SleepCommand(2));
			}
			
			m_levelLowerTick += FPS * 18.0f;
			m_levelUpperTick += FPS * 22.0f;
		}
		break;


	case LEVEL_EXPLODE:
		{
			for (int i = 0; i < 3; i++) {
				m_commands.push_back(new NormalEnemyAtPointCommand(
					vec2(0.f, 0.f), 50, 1));
				m_commands.push_back(new SleepCommand(60));
			}

			m_levelLowerTick += FPS * 10.0f;
			m_levelUpperTick += FPS * 13.0f;
		}
		break;

	case LEVEL_EXPLODE_HARD:
		{
			for (int i = 0; i < 3; i++) {
				m_commands.push_back(new NormalEnemyAtPointCommand(
					vec2(0.f, 0.f), 50, 2));
				m_commands.push_back(new SleepCommand(60));
			}

			m_levelLowerTick += FPS * 20.0f;
			m_levelUpperTick += FPS * 25.0f;
		}
		break;
	case LEVEL_ROUND0:
		{
			m_commands.push_back(new NormalEnemyAtNearPlayerCommand(
				200.f, 3, 1));
			m_levelLowerTick += FPS * 8.0f;
			m_levelUpperTick += FPS * 14.0f;

		}
		break;
	case LEVEL_ROUND1:
		{
			m_commands.push_back(new NormalEnemyAtNearPlayerCommand(
				200.f, 10, 1));
			m_levelLowerTick += FPS * 8.0f;
			m_levelUpperTick += FPS * 14.0f;

		}
		break;
	
	case LEVEL_ROUND2:
		{
			for (int i = 0; i < 2; i++) {
				m_commands.push_back(new NormalEnemyAtNearPlayerCommand(
					120.f, 20, 1));
				m_commands.push_back(new SleepCommand(60));
			}

			m_levelLowerTick += FPS * 10.5f;
			m_levelUpperTick += FPS * 15.5f;

		}
		break;

	case LEVEL_ROUND8:
		{
			for (int i = 0; i < 8; i++) {
				m_commands.push_back(new NormalEnemyAtNearPlayerCommand(
					120.f + (i % 2) * 125.0f, 10, 1));
				m_commands.push_back(new SleepCommand(30));
				if (i % 4 == 3) {
					m_commands.push_back(new SleepCommand(100));
				}
			}
			m_levelLowerTick += FPS * 15.0f;
			m_levelUpperTick += FPS * 25.0f;
		}
		break;

	case LEVEL_SPEED1:
		{
			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 5, 1, E_NORMAL_V * 3.0f));
			m_levelLowerTick += FPS * 9.0f;
			m_levelUpperTick += FPS * 14.0f;
		}
		break;

	case LEVEL_SPEED2:
		{
			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 5, 1, E_NORMAL_V * 3.0f));
			m_commands.push_back(new SleepCommand(30));
			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 5, 1, E_NORMAL_V * 3.0f));
			m_levelLowerTick += FPS * 9.0f;
			m_levelUpperTick += FPS * 14.0f;
		}
		break;

	case LEVEL_SPEED8:
		{
			for (int i = 0; i < 8; i++) {
				m_commands.push_back(new NormalEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 5, 1, E_NORMAL_V * 3.0f));
				m_commands.push_back(new SleepCommand(30));
			}

			m_levelLowerTick += FPS * 10.0f;
			m_levelUpperTick += FPS * 18.0f;
		}
		break;

	case LEVEL_BIG1:
		{
			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 1, 2, E_NORMAL_V * 0.8, E_LEN * 3));

			m_levelLowerTick += FPS * 8.0f;
			m_levelUpperTick += FPS * 13.0f;
		}
		break;
	
	case LEVEL_BIG2:
		{
			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 1, 2, E_NORMAL_V * 0.8, E_LEN * 3));
			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 1, 2, E_NORMAL_V * 1.2, E_LEN * 1));

			m_commands.push_back(new SleepCommand(120));

			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 1, 2, E_NORMAL_V * 0.8, E_LEN * 3));

			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 1, 2, E_NORMAL_V * 1.2, E_LEN * 1));

			m_levelLowerTick += FPS * 18.0f;
			m_levelUpperTick += FPS * 30.0f;
		}
		break;
	
	case LEVEL_BIG8:
		{
			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 2, 2, E_NORMAL_V * 0.8, E_LEN * 3));

			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 2, 2, E_NORMAL_V * 1.2, E_LEN * 1));

			m_commands.push_back(new SleepCommand(120));

			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 2, 2, E_NORMAL_V * 0.8, E_LEN * 3));

			m_commands.push_back(new NormalEnemyAtPointCommand(
				vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 2, 2, E_NORMAL_V * 1.2, E_LEN * 1));

			m_levelLowerTick += FPS * 17.0f;
			m_levelUpperTick += FPS * 33.0f;
		}
		break;

	case LEVEL_BARA1:
		{
			vec2 velo = Pcs(30.f,
					(f32)RandRange<i32>(0, 8) * 0.25f * PI + RandRangeMid(PI * 0.125, 0.03));
			vec2 pos = vec2(RandRangeMid(0.f, FIELD_Lf*2), RandRangeMid(0.f, FIELD_Lf*2));
			for (int i = 0; i < 90; i++) {
				pos += velo;
				if (pos.x < -FIELD_Lf) {
					pos.x = -FIELD_Lf;
					velo.x = -velo.x;
				} else if (pos.x > FIELD_Lf) {
					pos.x = FIELD_Lf;
					velo.x = -velo.x;
				}
				if (pos.y < -FIELD_Lf) {
					pos.y = -FIELD_Lf;
					velo.y = -velo.y;
				} else if (pos.y > FIELD_Lf) {
					pos.y = FIELD_Lf;
					velo.y = -velo.y;
				}
				m_commands.push_back(new NormalEnemyAtPointCommand(
					pos, 1, 1, E_NORMAL_V * 0.0f, E_LEN));
				m_commands.push_back(new SleepCommand(3));
			}
			m_levelLowerTick += FPS * 20.0f;
			m_levelUpperTick += FPS * 31.0f;
		}
		break;

	case LEVEL_BARA2:
		{
			vec2 velo = Pcs(40.f,
					(f32)RandRange<i32>(0, 8) * 0.25f * PI + RandRangeMid(PI * 0.125, 0.03));
			vec2 pos = vec2(RandRangeMid(0.f, FIELD_Lf*2), RandRangeMid(0.f, FIELD_Lf*2));
			for (int i = 0; i < 140; i++) {
				pos += velo;
				if (pos.x < -FIELD_Lf) {
					pos.x = -FIELD_Lf;
					velo.x = -velo.x;
				} else if (pos.x > FIELD_Lf) {
					pos.x = FIELD_Lf;
					velo.x = -velo.x;
				}
				if (pos.y < -FIELD_Lf) {
					pos.y = -FIELD_Lf;
					velo.y = -velo.y;
				} else if (pos.y > FIELD_Lf) {
					pos.y = FIELD_Lf;
					velo.y = -velo.y;
				}
				m_commands.push_back(new NormalEnemyAtPointCommand(
					pos, 1, 1, E_NORMAL_V * 0.0, E_LEN));
				m_commands.push_back(new SleepCommand(2));
			}
			m_levelLowerTick += FPS * 22.0f;
			m_levelUpperTick += FPS * 33.0f;
		}
		break;

	case LEVEL_BARA8:
		{
			vec2 velo = Pcs(30.f,
					(f32)RandRange<i32>(0, 8) * 0.25f * PI + RandRangeMid(PI * 0.125, 0.03));
			vec2 pos = vec2(RandRangeMid(0.f, FIELD_Lf*2), RandRangeMid(0.f, FIELD_Lf*2));
			for (int i = 0; i < 200; i++) {
				pos += velo;
				if (pos.x < -FIELD_Lf) {
					pos.x = -FIELD_Lf;
					velo.x = -velo.x;
				} else if (pos.x > FIELD_Lf) {
					pos.x = FIELD_Lf;
					velo.x = -velo.x;
				}
				if (pos.y < -FIELD_Lf) {
					pos.y = -FIELD_Lf;
					velo.y = -velo.y;
				} else if (pos.y > FIELD_Lf) {
					pos.y = FIELD_Lf;
					velo.y = -velo.y;
				}
				m_commands.push_back(new NormalEnemyAtPointCommand(
					pos, 1, 1, E_NORMAL_V * 0.0, E_LEN));

			}

			m_commands.push_back(new SleepCommand(120));

			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(250.f, 30, 1));

			m_levelLowerTick += FPS * 17.0f;
			m_levelUpperTick += FPS * 22.0f;
		}
		break;

	case LEVEL_BOSS1:
		m_commands.push_back(
			new DashEnemyAtPointCommand(vec2(0.f, 0.f), 1, 2, E_LEN * 1.5));
		m_levelLowerTick += FPS * 15.0f;
		m_levelUpperTick += FPS * 22.0f;

		break;
	case LEVEL_BOSS2:
		m_commands.push_back(
			new DashEnemyAtPointCommand(vec2(0.f, 0.f), 1, 2, E_LEN * 1.5));
		m_commands.push_back(
			new SleepCommand(90));
		m_commands.push_back(
			new DashEnemyAtPointCommand(vec2(0.f, 0.f), 1, 2, E_LEN * 1.5));

		m_levelLowerTick += FPS * 20.0f;
		m_levelUpperTick += FPS * 30.0f;

		break;

	case LEVEL_BOSS2_OTOMO:
		m_commands.push_back(
			new DashEnemyAtPointCommand(vec2(0.f, 0.f), 1, 2, E_LEN * 1.5));
		for (int i = 0; i < 3; i++) {
			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 3, 1));
			m_commands.push_back(
				new SleepCommand(30));
		}
		m_commands.push_back(
			new DashEnemyAtPointCommand(vec2(0.f, 0.f), 1, 2, E_LEN * 2));

		for (int i = 0; i < 3; i++) {
			m_commands.push_back(
				new NormalEnemyAtNearPlayerCommand(200.f, 2, 1));
			m_commands.push_back(
				new SleepCommand(80));

			m_levelLowerTick += FPS * 25.0f;
			m_levelUpperTick += FPS * 30.0f;
		}
		break;
	case LEVEL_BOSS3:
		{
			for (int i = 0; i < 3; i++) {
				m_commands.push_back(
					new DashEnemyAtPointCommand(
					vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 1, 3, E_LEN * 1.8));
			}
			m_levelLowerTick += FPS * 25.0f;
			m_levelUpperTick += FPS * 30.0f;
		}
		break;
	case LEVEL_DEMO:
		for (int i = 0; i < 100; i++) {
			switch (i % 3) {
			case 0:
				m_commands.push_back(
					new NormalEnemyAtNearPlayerCommand(200.f, 2, 1, 300.f));
				break;
			case 1:
				m_commands.push_back(
					new DashEnemyAtPointCommand(vec2(RandRangeMid(0.f, FIELD_Lf), RandRangeMid(0.f, FIELD_Lf)), 1, 1, E_LEN * 2));
				break;
			case 2:
				m_commands.push_back(
					new NormalEnemyAtPointCommand(vec2(), 10, 1));
				break;
			}

			m_commands.push_back(new SleepCommand(120));
		}

		break;

	}
}

