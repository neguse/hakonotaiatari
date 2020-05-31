
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

	App::renderText(vec2(1, 16), 0xffffffff, L"�͂��̂��������� �Ƃ́c!?");

	App::renderText(vec2(1, 14), 0xffffffff, L"����������� �G�� �ӂ��Ƃ΂�!");

	App::renderText(vec2(1, 12), 0xffffffff,	L"���N���b�N �� ����������U��");
	App::renderText(vec2(1, 11), 0xffffffff,	L"���������蒆�ɓG�ɂԂ����");
	App::renderText(vec2(1, 10), 0xffffffff,	L" �G���ӂ��Ƃ΂��܂�");
	App::renderText(vec2(1, 9), 0xffffffff,		L"���������蒆�ɓG�ɂԂ������");
	App::renderText(vec2(1, 8), 0xffffffff,		L" �G�ɂӂ��Ƃ΂���܂�");
	
	App::renderText(vec2(1, 6), 0xffffffff, L"3�� �G�ɂӂ��Ƃ΂�����");
	App::renderText(vec2(4, 5), 0xffff0000, L"�Q�[���I�[�o�[");

	if ((m_tick / 40) % 2 == 0) {
		App::renderText(vec2(1, 3), 0xffffffff, L"���N���b�N �� �͂��܂�܂�");
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
