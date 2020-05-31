
#pragma once

class Time {
public:

	// 指定時間処理を止める
	static void Sleep(u32 msec)
	{
		Sleep(msec);
	}


	// 現在の時間をミリ秒単位で取得
	static u32 GetTime()
	{
		return timeGetTime();
	}

}; // class Time
