
#include "precompile.h"
#include "dll_interface.h"
#include "app.h"
#include "app_thread.h"

namespace {
	static AppThread s_appThread;
} // namespace

extern "C" {

	LIBRARY_API void fs1_run()
	{
		s_appThread.run();
	}

	LIBRARY_API void fs1_start()
	{
		App::start();
	}

	LIBRARY_API void fs1_stop()
	{
		App::stop();
	}

	LIBRARY_API void fs1_set_fps( u32 fps )
	{
		App::setFps(fps);
	}
	LIBRARY_API u32 fs1_get_fps()
	{
		return App::getFps();
	}
	LIBRARY_API void fs1_add_life()
	{
		App::addLife();
	}


}
