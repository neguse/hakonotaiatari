
#pragma once

#include "state.h"

class TestState : public IState
{

public:

	virtual void onInit();
	virtual void onUpdate(f32 dt);
	virtual IState* nextState();


}; // class TestState

