
#pragma once

#define LIBRARY_API __declspec(dllexport)

extern "C" {

	LIBRARY_API void fs1_run();

	LIBRARY_API void fs1_start();
	LIBRARY_API void fs1_stop();

	LIBRARY_API void fs1_set_fps(u32 fps);
	LIBRARY_API u32 fs1_get_fps();
	LIBRARY_API void fs1_add_life();

} // extern C
