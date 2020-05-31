
#include "precompile.h"
#include "game_process.h"

#include "state.h"

void GameProcess::onInit()
{
}

void GameProcess::onUpdate(f32 dt)
{
	if (m_pState) {
		m_pState->onUpdate(dt);
		IState* pNext = m_pState->nextState();
		if (pNext) {
			trans(pNext);
		}
	}
}

void GameProcess::onRender()
{
	if (m_pState) {
		m_pState->onRender();
	}
}

void GameProcess::trans(IState* pState)
{
	if (m_pState) m_pState->onExit();

	if (pState != NULL) {
		m_pState = pState;
	} else {
		m_pState = NULL;
	}

	if (m_pState != NULL) {
		m_pState->onInit();
	}
}

