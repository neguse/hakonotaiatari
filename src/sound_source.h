
#pragma once

#include "sequencer.h"
#include "wave_out.h"
#include "tone_generator.h"

class SoundSource
: public IWaveOutHandler
, public ISequencerCommandHandler
{

	static const u32 TONE_GENERATOR_COUNT = 8;

private:

	ToneGenerator m_toneGenerator[TONE_GENERATOR_COUNT];
	Sequencer m_sequencer;

public:

	SoundSource();
	~SoundSource();

	void reset();

	ToneGenerator& getGenerator(u32 index)
	{
		return m_toneGenerator[index];
	}

	void onPlay(WAVE_BUFFER_DATA* pBuffer, const WAVE_BUFFER_TYPE& type);

	void onCommand(int index, const ROW_COMMAND& command);

}; // class SoundSource

