
#include "precompile.h"
#include "sequencer.h"


static const u32 TONE_FREQ[TONE_MAX] = {
	0,
	523,
	554,
	587,
	622,
	659,
	698,
	740,
	784,
	831,
	880,
	932,
	988,
}; // TONE_FREQ


// コンストラクタ
Sequencer::Sequencer(ISequencerCommandHandler* pHandler)
	: m_pHandler(pHandler)
{
	reset();
}

void Sequencer::reset()
{
	m_row = 0;
	m_step = 0;
	m_steps = 5800;

	for (int i = 0; i < ROW_MAX; i++) {
		m_rowCommand[i].tone = TONE_NONE;
		m_rowCommand[i].octave = 2;
		m_rowCommand[i].volume = 0x30;
	}

	// かえるのうた
	/*
	m_rowCommand[0x00].tone = TONE_C;
	m_rowCommand[0x01].tone = TONE_NONE;
	m_rowCommand[0x04].tone = TONE_D;
	m_rowCommand[0x05].tone = TONE_NONE;
	m_rowCommand[0x08].tone = TONE_E;
	m_rowCommand[0x09].tone = TONE_NONE;
	m_rowCommand[0x0c].tone = TONE_F;
	m_rowCommand[0x0d].tone = TONE_NONE;

	m_rowCommand[0x10].tone = TONE_E;
	m_rowCommand[0x11].tone = TONE_NONE;
	m_rowCommand[0x14].tone = TONE_D;
	m_rowCommand[0x15].tone = TONE_NONE;
	m_rowCommand[0x18].tone = TONE_C;
	m_rowCommand[0x19].tone = TONE_NONE;
	m_rowCommand[0x1c].tone = TONE_NONE;
	m_rowCommand[0x1d].tone = TONE_NONE;

	m_rowCommand[0x20].tone = TONE_E;
	m_rowCommand[0x21].tone = TONE_NONE;
	m_rowCommand[0x24].tone = TONE_F;
	m_rowCommand[0x25].tone = TONE_NONE;
	m_rowCommand[0x28].tone = TONE_G;
	m_rowCommand[0x29].tone = TONE_NONE;
	m_rowCommand[0x2c].tone = TONE_A;
	m_rowCommand[0x2d].tone = TONE_NONE;

	m_rowCommand[0x30].tone = TONE_G;
	m_rowCommand[0x31].tone = TONE_NONE;
	m_rowCommand[0x34].tone = TONE_F;
	m_rowCommand[0x35].tone = TONE_NONE;
	m_rowCommand[0x38].tone = TONE_E;
	m_rowCommand[0x39].tone = TONE_NONE;
	m_rowCommand[0x3c].tone = TONE_NONE;
	m_rowCommand[0x3d].tone = TONE_NONE;

	m_rowCommand[0x40].tone = TONE_C;
	m_rowCommand[0x41].tone = TONE_NONE;
	m_rowCommand[0x44].tone = TONE_NONE;
	m_rowCommand[0x45].tone = TONE_NONE;
	m_rowCommand[0x48].tone = TONE_C;
	m_rowCommand[0x49].tone = TONE_NONE;
	m_rowCommand[0x4c].tone = TONE_NONE;
	m_rowCommand[0x4d].tone = TONE_NONE;

	m_rowCommand[0x50].tone = TONE_C;
	m_rowCommand[0x51].tone = TONE_NONE;
	m_rowCommand[0x54].tone = TONE_NONE;
	m_rowCommand[0x55].tone = TONE_NONE;
	m_rowCommand[0x58].tone = TONE_C;
	m_rowCommand[0x59].tone = TONE_NONE;
	m_rowCommand[0x5c].tone = TONE_NONE;
	m_rowCommand[0x5d].tone = TONE_NONE;

	m_rowCommand[0x60].tone = TONE_C;
	m_rowCommand[0x61].tone = TONE_NONE;
	m_rowCommand[0x62].tone = TONE_C;
	m_rowCommand[0x63].tone = TONE_NONE;
	m_rowCommand[0x64].tone = TONE_D;
	m_rowCommand[0x65].tone = TONE_NONE;
	m_rowCommand[0x66].tone = TONE_D;
	m_rowCommand[0x67].tone = TONE_NONE;
	m_rowCommand[0x68].tone = TONE_E;
	m_rowCommand[0x69].tone = TONE_NONE;
	m_rowCommand[0x6a].tone = TONE_E;
	m_rowCommand[0x6b].tone = TONE_NONE;
	m_rowCommand[0x6c].tone = TONE_F;
	m_rowCommand[0x6d].tone = TONE_NONE;
	m_rowCommand[0x6e].tone = TONE_F;
	m_rowCommand[0x6f].tone = TONE_NONE;

	m_rowCommand[0x70].tone = TONE_E;
	m_rowCommand[0x71].tone = TONE_NONE;
	m_rowCommand[0x74].tone = TONE_D;
	m_rowCommand[0x75].tone = TONE_NONE;
	m_rowCommand[0x78].tone = TONE_C;
	m_rowCommand[0x79].tone = TONE_NONE;
	m_rowCommand[0x7c].tone = TONE_NONE;
	m_rowCommand[0x7d].tone = TONE_NONE;
	*/
}

void Sequencer::step()
{
	m_step++;
	if (m_step > m_steps) {
		m_step -= m_steps;

		// コマンド実行
		if (m_pHandler != NULL) {
			m_pHandler->onCommand(0, m_rowCommand[m_row]);
		}

		m_row++;
		if (m_row >= ROW_MAX) {
			m_row = 0;
		}
	}
}

u32 Sequencer::freq(u32 tone, u32 octave)
{
	return TONE_FREQ[tone] * std::pow((double)2, (double)octave - 2);
}
