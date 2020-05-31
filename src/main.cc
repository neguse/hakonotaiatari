// main.cc

#include "precompile.h"
#include "app.h"
#include <Shlobj.h>

#include <string>

#define STANDALONE 1

#if STANDALONE

std::string GetApplicationLogPath()
{
	std::string path = getenv("appdata");
	path += APP_LOG_DIR;
	std::string cmd = std::string("mkdir ") + "\"" + path + "\"";
	// system(cmd.c_str());
	SHCreateDirectoryExA(NULL, path.c_str(), NULL);
	return path;
}

int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, "hakotai_1_0_");
	if(GetLastError() == ERROR_ALREADY_EXISTS) {
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
		u32 ret = MessageBox(NULL,
			"あ…ごめんなさい\n多重起動、できないんですよ",
			"はこたいからのおしらせ", MB_YESNO);
		if (ret != IDYES) {
			MessageBox(NULL,
				"そう言われましても…\nセーブデータっぽいファイルがあれものでして…",
				"はこたいからのおしらせ", MB_OK);
		}
		return 0;
	}

#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif
	std::string path = GetApplicationLogPath();
	google::InitGoogleLogging(lpCmdLine, path.c_str());

	bool setup_success = App::setup();

	if (setup_success) {
		App::run();
	}

	App::teardown();

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);

	return 0;
}

#else

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}



#endif

