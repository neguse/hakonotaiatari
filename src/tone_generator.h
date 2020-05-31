
#pragma once

class ToneGenerator
{

	static const u32 SAMPLE_MAX = 32;

private:

	u32 m_frequency;
	u32 m_step;
	u32 m_frame;
	u32 m_volume;
	u8 m_samples[SAMPLE_MAX];
	u8 m_sampleCount;

	void updateStep();

public:

	ToneGenerator();
	~ToneGenerator();

	void reset();

	void setFrequency(u32 frequency);
	void setVolume(u32 volume);
	void setSamples(const u8* pSample, u32 count);

	double getSample();

	void step();

}; // class ToneGenerator

