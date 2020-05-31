
#pragma once

#include "state.h"
#include "record.h"

class RecordState : public IState
{
private:
	u32 m_tick;
	RecordCalcData m_data;

	std::vector<RecordData> m_topData;

public:

	virtual void onInit();
	virtual void onUpdate(f32 dt);
	virtual void onRender();
	virtual void onExit();
	virtual IState* nextState();

}; // class GameState

