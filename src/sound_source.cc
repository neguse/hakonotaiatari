
#include "precompile.h"
#include "sound_source.h"



SoundSource::SoundSource()
	: m_sequencer(this)
{
	reset();
}

SoundSource::~SoundSource()
{
}

void SoundSource::reset()
{
	for (int i = 0; i < TONE_GENERATOR_COUNT; i++) {
		getGenerator(i).reset();
	}
}

void SoundSource::onPlay(WAVE_BUFFER_DATA* pBuffer, const WAVE_BUFFER_TYPE& type)
{
	u32 samples = type.samples();

	for (int f = 0; f < samples; f++) {
		m_sequencer.step();

		double t = 0.0;
		for (int i = 0; i < TONE_GENERATOR_COUNT; i++) {
			ToneGenerator& generator = getGenerator(i);
			t += generator.getSample();
			generator.step();
		}

		if (WAVE_BITS == 8) {
			u8* pBuff = (u8*)&pBuffer[f * type.sampleSize()];
			u8 v = glm::clamp<u8>(t, 0x00, 0xff);
			*pBuff = v;
			if (WAVE_CHANNELS == 2) {
				pBuff++;
				*pBuff = v;
			}
		} else if (WAVE_BITS == 16) {
			i16* pBuff = (i16*)&pBuffer[f * type.sampleSize()];
			i16 v = t * 128;
			*pBuff = v;
			if (type.channels == 2) {
				pBuff++;
				*pBuff = v;
			}
		} else {
			LOG(FATAL) << "bits must be 8 or 16.";
		}

	}
}

void SoundSource::onCommand(int index, const ROW_COMMAND& command)
{
	ToneGenerator& g = getGenerator(index + 1);
	g.setVolume(command.volume);
	u32 freq = Sequencer::freq(command.tone, command.octave);
	g.setFrequency(freq);
}

