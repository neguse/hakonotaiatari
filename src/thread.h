
#pragma once

class Thread {
private:
	HANDLE m_hThread;
	u32 m_id;

public:

	Thread()
		: m_hThread(NULL)
		, m_id(0)
	{
	}

	virtual ~Thread()
	{
	}

	// ������ĂԂƃX���b�h������
	virtual Retval run();

	// ������I�[�o�[���C�h����Ə�����ς�����
	virtual u32 proc()
	{
		return 0;
	}

	static unsigned WINAPI ThreadProc(void* pArgument);

}; // class Thread

