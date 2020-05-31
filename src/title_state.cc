
#include "precompile.h"
#include "title_state.h"

#include "app.h"
#include "mouse.h"
#include "tutorial_state.h"

namespace {

static const u32 SUBTITLE_MAX = 15;
static const wchar_t* SUBTITLE[SUBTITLE_MAX] = {
	L"-������ ���悤�Ȃ�-",
	L"-���������� �͂��߂܂���-",
	L"-�͂�ɂ�� �₷-",
	L"-�������ӂ� �݂�-",
	L"-���� �ł��Ƃ���ł�-",
	L"-������ ����� ���Ȃ��Ȃ���-",
	L"-���ǂ�ǂ�ǂ� ���΂΂�΂�-",
	L"-�ЂƂ�ЂƂ�� �ЂƂ݂���-",
	L"-����ǂ���� �т�����-",
	L"-�͂����� �Ȃ�-",
	L"-���� ����-",
	L"-�ЂƂɂ������ �͂��ɂ�����-",
	L"-���E�́[�ǂ���-",
	L"-�s�n�e�t �k�n�u�d-",
	L"�i�E�Q�E�j",
};


static u32 subtitle_id = RandRange<u32>(0, SUBTITLE_MAX - 1);

}


void TitleState::onInit()
{
	LOG(INFO) << "TitleState::onInit()";

	// App::setPlayWave(WAVE_BGM_INDEX, false, true);

	App::pCamera->setBehind(CAM_BEHIND_HIGH_TITLE, CAM_BEHIND_BACK_TITLE);
	App::pCamera->setLookat(vec3());
	App::pCamera->setEnableRotate(true);

	App::pRenderer->setGakugaku(1.0f);
	App::renderScore = false;

	App::setPlayWave(WAVE_TITLE_INDEX);

	m_tick = 0;
}

void TitleState::onUpdate(f32 dt)
{
	m_tick++;

	App::pCamera->update();
}

void TitleState::onExit()
{
	App::setPlayWave(WAVE_TITLE_INDEX, false);
}

void TitleState::onRender()
{
	App::renderText(vec2(5, 10), 0xffffffff, L"�͂� �� ����������");

	const wchar_t* subtitle = SUBTITLE[subtitle_id];
	u32 subtitle_len = wcslen(subtitle);
	App::renderText(vec2(10.f - (f32)subtitle_len * 0.5f, 7), 0xffffffff, subtitle);


	if ((m_tick / 30) % 4 < 3) {
		App::renderText(vec2(2, 5), 0xffffffff, L"���N���b�N �� �͂��܂�܂�");
		App::renderText(vec2(2, 4), 0xffffffff, L"F �L�[ �� �S��ʕ\�� �ɂȂ�܂�");
	}


	App::renderText(vec2(10, 2), 0xffffffff, L"(C)neguse 2012");
}

IState* TitleState::nextState()
{
	if (App::pMouse->getLButtonDown()) {
		return new TutorialState();
	}
	return NULL;
}
