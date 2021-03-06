
#include "precompile.h"
#include "tutorial_state.h"

#include "app.h"
#include "renderer.h"
#include "game_state.h"


void TutorialState::onInit()
{
	LOG(INFO) << "TutorialState::onInit()";
	m_tick = 0;

	swprintf(App::noticeBuffer, L"");

	App::initRecord();

	App::pCamera->setEnableRotate(false);
}

void TutorialState::onUpdate(f32 dt)
{
	App::pCamera->update();

	m_tick++;
}

void TutorialState::onRender()
{
	App::pRenderer->setGakugaku(1.f);

	App::renderText(vec2(1, 16), 0xffffffff, L"はこのたいあたり とは…!?");

	App::renderText(vec2(1, 14), 0xffffffff, L"たいあたりで 敵を ふっとばせ!");

	App::renderText(vec2(1, 12), 0xffffffff,	L"左クリック で たいあたり攻撃");
	App::renderText(vec2(1, 11), 0xffffffff,	L"たいあたり中に敵にぶつかると");
	App::renderText(vec2(1, 10), 0xffffffff,	L" 敵をふっとばせます");
	App::renderText(vec2(1, 9), 0xffffffff,		L"たいあたり中に敵にぶつかられると");
	App::renderText(vec2(1, 8), 0xffffffff,		L" 敵にふっとばされます");
	
	App::renderText(vec2(1, 6), 0xffffffff, L"3回 敵にふっとばされると");
	App::renderText(vec2(4, 5), 0xffff0000, L"ゲームオーバー");

	if ((m_tick / 40) % 2 == 0) {
		App::renderText(vec2(1, 3), 0xffffffff, L"左クリック で はじまります");
	}
}

void TutorialState::onExit()
{

}

IState* TutorialState::nextState()
{
	if (App::pMouse->getLButtonDown()) {
		return new GameState();
	} else {
		return NULL;
	}
}
