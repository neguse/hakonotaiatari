
#pragma once

enum Tone {
	TONE_NONE = 0,
	TONE_C,
	TONE_CM,
	TONE_D,
	TONE_DM,
	TONE_E,
	TONE_F,
	TONE_FM,
	TONE_G,
	TONE_GM,
	TONE_A,
	TONE_AM,
	TONE_B,
	TONE_MAX,
};

struct ROW_COMMAND
{
	u32 tone;
	u32 octave;
	u32 volume;
}; // struct ROW_COMMAND

class ISequencerCommandHandler
{
public:
	virtual void onCommand(int index, const ROW_COMMAND& command) = 0;
};

class Sequencer
{

	static const int ROW_MAX = 0x80;
	static const int ROW_PER_BEAT = 4;

	u32 m_row;
	u32 m_step;
	u32 m_steps;
	ROW_COMMAND m_rowCommand[ROW_MAX];

	ISequencerCommandHandler* m_pHandler;

private:

public:

	Sequencer(ISequencerCommandHandler* pHandler = NULL);

	~Sequencer()
	{ }

	void reset();

	void step();

	static u32 freq(u32 tone, u32 octave);

};

