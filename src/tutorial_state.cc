
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

	App::renderText(vec2(1, 16), 0xffffffff, L"‚Í‚±‚Ì‚½‚¢‚ ‚½‚è ‚Æ‚Íc!?");

	App::renderText(vec2(1, 14), 0xffffffff, L"‚½‚¢‚ ‚½‚è‚Å “G‚ð ‚Ó‚Á‚Æ‚Î‚¹!");

	App::renderText(vec2(1, 12), 0xffffffff,	L"¶ƒNƒŠƒbƒN ‚Å ‚½‚¢‚ ‚½‚èUŒ‚");
	App::renderText(vec2(1, 11), 0xffffffff,	L"‚½‚¢‚ ‚½‚è’†‚É“G‚É‚Ô‚Â‚©‚é‚Æ");
	App::renderText(vec2(1, 10), 0xffffffff,	L" “G‚ð‚Ó‚Á‚Æ‚Î‚¹‚Ü‚·");
	App::renderText(vec2(1, 9), 0xffffffff,		L"‚½‚¢‚ ‚½‚è’†‚É“G‚É‚Ô‚Â‚©‚ç‚ê‚é‚Æ");
	App::renderText(vec2(1, 8), 0xffffffff,		L" “G‚É‚Ó‚Á‚Æ‚Î‚³‚ê‚Ü‚·");
	
	App::renderText(vec2(1, 6), 0xffffffff, L"3‰ñ “G‚É‚Ó‚Á‚Æ‚Î‚³‚ê‚é‚Æ");
	App::renderText(vec2(4, 5), 0xffff0000, L"ƒQ[ƒ€ƒI[ƒo[");

	if ((m_tick / 40) % 2 == 0) {
		App::renderText(vec2(1, 3), 0xffffffff, L"¶ƒNƒŠƒbƒN ‚Å ‚Í‚¶‚Ü‚è‚Ü‚·");
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
