
#include "precompile.h"
#include "audio.h"

namespace {
	static const int CHANNELS = 1;
	static const int SAMPLES = 22100;
	static const int BITS = 8;
	static const int HEADER_SIZE = 40;
}

Audio::Audio()
	: m_pAudio(NULL)
	, m_pMaster(NULL)
{

}

Audio::~Audio()
{

}

Retval Audio::initialize()
{

	UINT32 flags = 0;
#if _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif
	IXAudio2* pAudio = NULL;
	{
		HRESULT result = XAudio2Create(&pAudio, flags);
		if (FAILED(result)) {
			LOG(ERROR) << "XAudio2Create()‚ÉŽ¸”s‚µ‚Ü‚µ‚½";
			return Retval(R_FAIL);
		}
	}

	IXAudio2MasteringVoice* pMaster = NULL;
	{
		HRESULT result = pAudio->CreateMasteringVoice(
			&pMaster, XAUDIO2_DEFAULT_CHANNELS, SAMPLES);
		if (FAILED(result)) {
			pAudio->Release();
		}
	}

	m_pAudio = pAudio;
	m_pMaster = pMaster;
	return Retval(R_SUCCESS);
}

Retval Audio::finalize()
{
	if (m_pAudio == NULL) {
		return Retval(R_FAIL_MULTIPLE_FINALIZE);
	}

	m_pMaster->DestroyVoice();
	m_pMaster = NULL;
	m_pAudio->Release();
	m_pAudio = NULL;

	return Retval(R_SUCCESS);
}

AudioWave* Audio::createWave( const void* pWaveBuffer, size_t waveBufferSize )
{
	AudioWave* pWave = new AudioWave();
	Retval ret = pWave->initialize(m_pAudio, pWaveBuffer, waveBufferSize);
	if (ret.isFail()) {
		delete pWave;
		return NULL;
	}
	return pWave;
}


AudioWave::AudioWave()
	: m_pSourceVoice(NULL)
	, m_pWaveBuffer(NULL)
	, m_waveBufferSize(0)
{
}

AudioWave::~AudioWave()
{
}

Retval AudioWave::initialize( IXAudio2* pAudio, const void* pWaveBuffer, size_t waveBufferSize )
{
	if (m_pSourceVoice != NULL) {
		return Retval(R_FAIL_MULTIPLE_INITIALIZE);
	}

	WAVEFORMATEX wf = {};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = CHANNELS;
	wf.nSamplesPerSec = SAMPLES;
	wf.nAvgBytesPerSec = SAMPLES * CHANNELS * BITS / 8;
	wf.nBlockAlign = BITS * CHANNELS / 8;
	wf.wBitsPerSample = BITS;
	wf.cbSize = 0;

	IXAudio2SourceVoice* pSourceVoice;
	HRESULT result = pAudio->CreateSourceVoice(&pSourceVoice, &wf, 0);
	if (FAILED(result)) {
		LOG(ERROR) << "CreateSourceVoice()ŒÄ‚Ño‚µ‚ÉŽ¸”s‚µ‚Ü‚µ‚½";
		return Retval(R_FAIL);
	}

	const char* pBuffer = reinterpret_cast<const char*>(pWaveBuffer) + HEADER_SIZE;

	m_pSourceVoice = pSourceVoice;
	m_pWaveBuffer = pBuffer + sizeof(size_t);
	m_waveBufferSize = *reinterpret_cast<const size_t*>(pBuffer);

	return Retval(R_SUCCESS);
}

Retval AudioWave::finalize()
{
	if (m_pSourceVoice == NULL) {
		return Retval(R_FAIL_MULTIPLE_FINALIZE);
	}

	m_pSourceVoice->DestroyVoice();
	m_pSourceVoice = NULL;

	return Retval(R_SUCCESS);
}

void AudioWave::play(bool doesLoop)
{
	XAUDIO2_BUFFER buf = {0};
	buf.pAudioData = (BYTE*)m_pWaveBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = m_waveBufferSize;
	if (doesLoop) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	} else {
		buf.LoopCount = 0;
	}

	{
		HRESULT result = m_pSourceVoice->SubmitSourceBuffer(&buf);
		if (FAILED(result)) {
			LOG(ERROR) << "SubmitSourceBuffer()ŒÄ‚Ño‚µ‚ÉŽ¸”s‚µ‚Ü‚µ‚½";
			return;
		}
	}
	{
		HRESULT result = m_pSourceVoice->Start(0);
		if (FAILED(result)) {
			LOG(ERROR) << "Start()ŒÄ‚Ño‚µ‚ÉŽ¸”s‚µ‚Ü‚µ‚½";
			return;
		}
	}
	
}

void AudioWave::stop()
{
	m_pSourceVoice->Stop(0);
	m_pSourceVoice->FlushSourceBuffers();

}

int AudioWave::stat()
{
	XAUDIO2_VOICE_STATE state;
	m_pSourceVoice->GetState(&state);
	return state.BuffersQueued > 0;

}
