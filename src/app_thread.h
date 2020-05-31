
#pragma once

#include "thread.h"

class AppThread : public Thread 
{

public:

	AppThread();
	virtual ~AppThread();

	virtual u32 proc();


}; // class AppThread
