
#pragma once

class IState
{
public:
	virtual void onInit()
	{
	}

	virtual void onExit()
	{
	}

	virtual void onUpdate(f32 dt)
	{
	}

	virtual void onRender()
	{
	}

	virtual IState* nextState()
	{
		return NULL;
	}

}; // class State

