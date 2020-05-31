
#include "precompile.h"
#include "game_state.h"

#include "app.h"
#include "renderer.h"
#include "record_state.h"
#include "player.h"

#include "normal_enemy.h"
#include "dash_enemy.h"

void GameState::onInit()
{
	LOG(INFO) << "GameState::onInit()";
	m_tick = 0;
	App::initRecord();
	App::pPlayer->onInit();

	swprintf(App::noticeBuffer, L"");

	App::initRecord();

	switch (RandRange<u32>(0, 2)) {
	case 0:
		App::setPlayWave(WAVE_BGM1_INDEX, true, true);
		break;
	case 1:
		App::setPlayWave(WAVE_BGM2_INDEX, true, true);
		break;
	case 2:
		App::setPlayWave(WAVE_BGM3_INDEX, true, true);
		break;
	}
	
	App::pCamera->setEnableRotate(false);

	App::renderScore = true;
	App::renderLife = true;
}

void GameState::onUpdate(f32 dt)
{
	App::pCamera->update();

	m_tick++;
	App::pPlayer->onUpdate(dt);

	m_generator.update(dt);

	if ((m_tick % 10 % 2) == 0 && ((double)std::rand() / RAND_MAX) * 40 < 1.0) {
		// newEnemy();
	}

	if (App::enemies.empty()) {
		// newEnemy();
	}

	for (std::list<Enemy*>::iterator it = App::enemies.begin(); it != App::enemies.end(); it++) {
		(*it)->onUpdate(dt);
	}

	// “G‚ÆŽ©‹@‚Ì“–‚½‚èˆ—
	if (App::pPlayer->isCollEnable()) {
		for (std::list<Enemy*>::iterator it = App::enemies.begin(); it != App::enemies.end(); it++) {
			if ((*it)->isCollEnable()) {
				if (Cube::IsCubeCollide(App::pPlayer, (*it))) {
					App::pPlayer->onCollide(*it);
					(*it)->onCollide(App::pPlayer);
				}
			}
		}
	}
	App::pPlayer->clampPosition();

	for (std::list<Enemy*>::iterator it1 = App::enemies.begin(); it1 != App::enemies.end(); it1++) {
		if ((*it1)->isCollEnable()) {
			for (std::list<Enemy*>::iterator it2 = it1; it2 != App::enemies.end(); it2++) {
				if (it1 != it2 && (*it2)->isCollEnable()) {
					if (Cube::IsCubeCollide(*it1, *it2)) {
						(*it1)->onCollide(*it2);
						(*it2)->onCollide(*it1);
					}
				}
			}
		}
	}

	for (std::list<Enemy*>::iterator it = App::enemies.begin(); it != App::enemies.end();) {
		Enemy* pEnemy = *it;
		if (pEnemy->stat() == E_ST_DEAD) {
			delete pEnemy;
			it = App::enemies.erase(it);
		} else {
			it++;
		}
	}

	if (App::pPlayer->stat() != P_ST_DEAD) {
		App::pRenderer->setGakugaku (Max<i32>(5 - App::pPlayer->life(), 0) * 0.2f);
	} else {
		App::pRenderer->setGakugaku(App::pRenderer->getGakugaku() + 0.001f);
	}

}

void GameState::onExit()
{

	App::pPlayer->onInit();

	for (std::list<Enemy*>::iterator it = App::enemies.begin(); it != App::enemies.end();) {
		Enemy* pEnemy = *it;
		delete pEnemy;
		it = App::enemies.erase(it);
	}

	swprintf(App::noticeBuffer, L"");
	App::pRenderer->setGakugaku(0.0f);

	App::setPlayWave(WAVE_BGM1_INDEX, false);
	App::setPlayWave(WAVE_BGM2_INDEX, false);
	App::setPlayWave(WAVE_BGM3_INDEX, false);

	App::renderLife = false;
}

IState* GameState::nextState()
{
	if (App::pPlayer->stat() == P_ST_DEAD && App::pMouse->getLButtonDown()) {
		return new RecordState();
	} else {
		return NULL;
	}
}

void GameState::onRender()
{
	m_generator.onRender();
}
