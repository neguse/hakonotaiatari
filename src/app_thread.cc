
#include "precompile.h"
#include "app_thread.h"
#include "app.h"


AppThread::AppThread()
{

}

AppThread::~AppThread()
{

}

u32 AppThread::proc()
{
	bool setup_success = App::setup();

	if (setup_success) {
		App::run();
	}

	App::teardown();


	return 0;
}

