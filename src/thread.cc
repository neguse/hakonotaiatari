
#include "precompile.h"
#include "thread.h"
#include <process.h>

Retval Thread::run()
{
	if (m_hThread != NULL){
		return Retval(R_FAIL_MULTIPLE_INITIALIZE);
	}

	uintptr_t handle = ::_beginthreadex(
		NULL,
		0,
		ThreadProc,
		(void*)this,
		0,
		NULL);

	if (handle == -1) {
		LOG(ERROR) << "ƒXƒŒƒbƒh‚Ì¶¬‚ÉŽ¸”s‚µ‚Ü‚µ‚½";
		return Retval(R_FAIL);
	}

	m_hThread = (HANDLE)handle;
	return Retval(R_SUCCESS);
}



unsigned WINAPI Thread::ThreadProc(void* pArgument)
{
	Thread* pThread = (Thread*)pArgument;
	u32 ret = pThread->proc();
	return (uintptr_t)ret;
}
