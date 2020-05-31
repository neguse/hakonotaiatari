
#pragma once

class IEnemyGeneratorCommand {

public:
	virtual bool done() const = 0;
	virtual void step(f32 dt) = 0;

}; // class EnemyGenratorCommand

class EnemyGenerator {

private:
	u32 m_levelTick;
	u32 m_levelUpperTick;
	u32 m_levelLowerTick;
	u32 m_upperCount;
	u32 m_centerCount;
	u32 m_lowerCount;
	u32 m_level; // åªç›ÇÃÉåÉxÉã
	f32 m_eval;
	f32 m_evalMean;
	std::list<IEnemyGeneratorCommand*> m_commands;

public:
	EnemyGenerator();
	virtual ~EnemyGenerator();

	void update(f32 dt);
	void onRender();
	void nextLevel();
	u32 getLevelId(u32 level, f32 eval);
	void createLevel(u32 id);
};

