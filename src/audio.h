
#pragma once

class AudioWave;

class Audio {

	IXAudio2* m_pAudio;
	IXAudio2MasteringVoice* m_pMaster;

public:

	Audio();
	virtual ~Audio();

	Retval initialize();
	Retval finalize();

	AudioWave* createWave(const void* pWaveBuffer, size_t waveBufferSize);

}; // class Audio

class AudioWave {
	IXAudio2SourceVoice* m_pSourceVoice;
	const void* m_pWaveBuffer;
	size_t m_waveBufferSize;

public:
	AudioWave();
	virtual ~AudioWave();

	Retval initialize(IXAudio2* pAudio, const void* pWaveBuffer, size_t waveBufferSize);
	Retval finalize();

	void play(bool doesLoop);
	void stop();
	int stat();
};
