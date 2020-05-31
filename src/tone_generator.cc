
#include "precompile.h"
#include "tone_generator.h"

#include "wave_out.h"

namespace {
	static const u32 GETA = 0x10;
}

void ToneGenerator::updateStep()
{
	m_step = (m_frequency * m_sampleCount << GETA) / WAVE_SAMPLE_RATE;
}

ToneGenerator::ToneGenerator()
{
	reset();
}

ToneGenerator::~ToneGenerator()
{ }

void ToneGenerator::reset()
{

	// TODO: リセット時のプリセット波形はいらないかも
	/*
	static const u8 samples[SAMPLE_MAX] = {
		0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
		0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xf0, 0xff,
		0xff, 0xf0, 0xd0, 0xc0, 0xb0, 0xa0, 0x90, 0x80,
		0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00,
	};
	*/

 static const u8 samples[SAMPLE_MAX] = {
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
 };

	setFrequency(400);
	setVolume(0);
	setSamples(samples, SAMPLE_MAX);
}

void ToneGenerator::setFrequency(u32 frequency)
{
	m_frequency = frequency;
	updateStep();
}

void ToneGenerator::setVolume(u32 volume)
{
	m_volume = volume;
}

void ToneGenerator::setSamples(const u8* pSample, u32 count)
{
	count = std::min(count, (u32)SAMPLE_MAX); // 最大でもMAX個まで
	memcpy(m_samples, pSample, count);
	m_sampleCount = count;

	updateStep();
}

double ToneGenerator::getSample()
{
	u32 size = 1;
	if (WAVE_BITS == 16) {
		size <<= 1;
	}
	if (WAVE_CHANNELS == 2) {
		size <<= 1;
	}

	u32 sample_idx = (m_frame >> GETA) % m_sampleCount;
	double t = ((double)m_samples[sample_idx] - 127.5) / 127.5;

	return t * m_volume;
}

void ToneGenerator::step()
{
		m_frame += m_step;
}

