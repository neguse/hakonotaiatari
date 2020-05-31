
#include "precompile.h"
#include "wave_out.h"

Retval WaveOut::initialize(const WAVE_BUFFER_TYPE& type, IWaveOutHandler* pHandler)
{

	WAVEFORMATEX wf;
	memset(&wf, 0, sizeof(wf));

	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = type.channels;
	wf.wBitsPerSample = type.bits;
	wf.nBlockAlign = (type.channels * wf.wBitsPerSample) / 8;
	wf.nSamplesPerSec = type.sampleRate;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

	MMRESULT result = waveOutOpen(&m_hWaveOut,
			WAVE_MAPPER,
			&wf,
			(DWORD)OnCallback,
			(DWORD)this,
			CALLBACK_FUNCTION);

	if (result != MMSYSERR_NOERROR) {
		return Retval(R_FAIL);
	}

	for (int i = 0; i < WAVE_BUFFER_COUNT; i++) {
		m_pBuffers[i] = (WAVE_BUFFER_DATA*)malloc(type.size);

		WAVEHDR& header = m_waveHeader[i];
		Clear(header);
		header.lpData = (LPSTR)m_pBuffers[i];
		header.dwBufferLength = type.size;
		header.dwFlags = 0;
		header.dwUser = i;
		header.dwLoops = 0;

		MMRESULT result = waveOutPrepareHeader(
				m_hWaveOut,
				&m_waveHeader[i],
				sizeof(m_waveHeader[i]));
	}

	m_pHandler = pHandler;
	m_bufferType = type;

	return Retval(R_SUCCESS);
}

void WaveOut::onCallback(UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
	if (uMsg == WOM_DONE && m_status == WAVEOUT_STATUS_PLAY) {
		m_playingBufferIndex = (m_playingBufferIndex + 1) % WAVE_BUFFER_COUNT;

		m_pHandler->onPlay(m_pBuffers[m_playingBufferIndex], m_bufferType);

		waveOutWrite(
				m_hWaveOut,
				&m_waveHeader[m_playingBufferIndex],
				sizeof(m_waveHeader[m_playingBufferIndex]));
	}
}

void CALLBACK WaveOut::OnCallback(
		HWAVEOUT hwo, UINT uMsg,
		DWORD* dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	WaveOut* pWaveOut = (WaveOut*)dwInstance;
	pWaveOut->onCallback(uMsg, dwParam1, dwParam2);
}

Retval WaveOut::finalize()
{
	if (m_hWaveOut == NULL) {
		return Retval(R_FAIL_MULTIPLE_FINALIZE);
	}

	stop();
	
	for (int i = 0; i < WAVE_BUFFER_COUNT; i++) {
		MMRESULT result = waveOutUnprepareHeader(
				m_hWaveOut,
				&m_waveHeader[i],
				sizeof(m_waveHeader[i]));
		free(m_pBuffers[i]);
		m_pBuffers[i] = NULL;
	}

	waveOutClose(m_hWaveOut);
	m_hWaveOut = NULL;

	return Retval(R_SUCCESS);
}

Retval WaveOut::play()
{
	m_status = WAVEOUT_STATUS_PLAY;
	for (int i = 0; i < WAVE_BUFFER_COUNT; i++) {
		waveOutWrite(
				m_hWaveOut,
				&m_waveHeader[i],
				sizeof(m_waveHeader[i]));
		m_playingBufferIndex = i;
	}

	return Retval(R_SUCCESS);
}

Retval WaveOut::stop()
{
	m_status = WAVEOUT_STATUS_STOP;
	waveOutReset(m_hWaveOut);
	return Retval(R_SUCCESS);
}

