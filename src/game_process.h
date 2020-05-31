
#pragma once

class IState;

class GameProcess
{

protected:
	IState* m_pState;

public:

	GameProcess()
	: m_pState(NULL)
	{
	}

	virtual ~GameProcess()
	{
	}

	virtual void onInit();

	virtual void onUpdate(f32 dt);

	virtual void onRender();

	virtual void trans(IState* pState);

}; // class GameProcess

