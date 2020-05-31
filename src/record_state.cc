
#include "precompile.h"
#include "record_state.h"

#include "app.h"
#include "renderer.h"
#include "title_state.h"


void RecordState::onInit()
{
	LOG(INFO) << "RecordState::onInit()";
	m_tick = 0;

	swprintf(App::noticeBuffer, L"");

	if (!App::pRecordTable->getRecordCalcData(App::record, m_data)) {
		LOG(ERROR) << "成績の計算に失敗しました";
	}

	if (!App::pRecordTable->getTopRecord(5, m_topData)) {
		LOG(ERROR) << "トップランクの取得に失敗しました";
	}

	if ((m_topData.size() > 0)
		&& (m_topData[0].id == App::record.id)
		&& (App::record.score > 0))
	{
		// TOPなので曲をかえる
		App::setPlayWave(WAVE_RESULT_HIGH_INDEX, true, true);
	} else {
		App::setPlayWave(WAVE_RESULT_INDEX, true, true);
	}

}

void RecordState::onUpdate(f32 dt)
{
	App::pCamera->update();

	m_tick++;
}

void RecordState::onRender()
{
	App::pRenderer->setGakugaku(1.f);

	wchar_t buf[1024];

	// swprintf(buf, L"順位: %I64d / %I64d", m_data.rank, m_data.total);
// 	App::renderText(vec2(1, 16), 0xffffffff, buf);
// 
// 	swprintf(buf, L"平均点: %2.2lf", m_data.mean);
// 	App::renderText(vec2(1, 15), 0xffffffff, buf);
// 
// 	swprintf(buf, L"偏差値: %2.2lf", m_data.standard);
// 	App::renderText(vec2(1, 14), 0xffffffff, buf);

	swprintf(buf, L"つわものども");
	App::renderText(vec2(1, 17), 0xffffffff, buf);

	for (int i = 0; i < m_topData.size(); i++) {
		RecordData& data = m_topData[i];

		u32 color;
		vec2 d;
		if (data.id == App::record.id) {
			if ((m_tick / 2) % 2 == 0) {
				color = 0xffff0000;
				d.x += RandRange(0.f, 0.1f);
				d.y += RandRange(0.f, 0.1f);
			} else {
				color = 0xff000000;
			}
			color = Color(0xff, RandRange(0x80, 0xff), RandRange(0x80, 0xff), RandRange(0x80, 0xff));
		} else {
			color = 0xffffffff;
		}
		
		swprintf(buf, L"%d位: %12I64d", i + 1, data.score);
		App::renderText(vec2(5, 16 - i * 3) + d, color, buf);
		swprintf(buf, L"%S", data.inserted_at.c_str());
		App::renderText(vec2(7, 15 - i * 3) + d, color, buf);

	}
}

void RecordState::onExit()
{
	App::setPlayWave(WAVE_RESULT_INDEX, false, true);
	App::setPlayWave(WAVE_RESULT_HIGH_INDEX, false, true);

}

IState* RecordState::nextState()
{
	if (App::pMouse->getLButtonDown()) {
		return new TitleState();
	} else {
		return NULL;
	}
}
