
#pragma once

static const u32 WAVE_SAMPLE_RATE = 44100;
static const u32 WAVE_CHANNELS = 2;
static const u32 WAVE_BUFFER_SIZE = 4096;
static const u32 WAVE_BUFFER_COUNT = 3;
static const u32 WAVE_BITS 	= 16;

typedef u8 WAVE_BUFFER_DATA;

struct WAVE_BUFFER_TYPE
{
	u32 size;
	u32 channels;
	u32 bits;
	u32 sampleRate;

	WAVE_BUFFER_TYPE(
			u32 _size = WAVE_BUFFER_SIZE,
			u32 _channels = WAVE_CHANNELS,
			u32 _bits = WAVE_BITS,
			u32 _sampleRate = WAVE_SAMPLE_RATE)
		: size(_size)
		, channels(_channels)
		, bits(_bits)
		, sampleRate(_sampleRate)
	{ }

	u32 samples() const
	{
		return size / sampleSize();
	}

	u32 bytes() const
	{
		return (bits / 8);
	}

	u32 sampleSize() const
	{
		return channels * bytes();
	}

}; // struct WAVE_BUFFER_TYPE

class IWaveOutHandler
{
private:

public:
	IWaveOutHandler()
	{ }

	virtual ~IWaveOutHandler()
	{ }

	virtual void onPlay(WAVE_BUFFER_DATA* pBuffer, const WAVE_BUFFER_TYPE& type)
	{ }

}; // class IWaveOutHandler

class WaveOut
{
private:

	HWAVEOUT m_hWaveOut;

	WAVE_BUFFER_TYPE m_bufferType;

	u32 m_playingBufferIndex;

	WAVEHDR m_waveHeader[WAVE_BUFFER_COUNT];
	WAVE_BUFFER_DATA* m_pBuffers[WAVE_BUFFER_COUNT];

	IWaveOutHandler* m_pHandler;

	u32 m_status;
	enum {
		WAVEOUT_STATUS_NONE,
		WAVEOUT_STATUS_STOP,
		WAVEOUT_STATUS_PLAY,
	};

public:

	WaveOut()
		: m_hWaveOut(NULL)
		, m_playingBufferIndex(0)
		, m_status(WAVEOUT_STATUS_NONE)
		, m_pHandler(NULL)
	{
	}

	~WaveOut()
	{
	}

	Retval initialize(const WAVE_BUFFER_TYPE& type, IWaveOutHandler* pHandler);
	Retval finalize();

	void onCallback(UINT uMsg, DWORD dwParam1, DWORD dwParam2);

	static void CALLBACK OnCallback(
			HWAVEOUT hwo, UINT uMsg,
			DWORD* dwInstance, DWORD dwParam1, DWORD dwParam2);

	Retval play();
	Retval stop();

	HWAVEOUT getWaveOut() const
	{
		return m_hWaveOut;
	}

}; // class WaveOut

