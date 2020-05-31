
#pragma once

#include "particle.h"
#include "particle_group.h"
#include "camera.h"
#include "mouse.h"
#include "renderer.h"
#include "sqlite3_connector.h"

#include "player.h"
#include "enemy.h"
#include "record.h"

class Window;
class DIBSection;
class Mouse;
class WaveOut;
class SoundSource;
class GameProcess;
class Camera;
class ParticleGroup;
class Player;
class Enemy;
class Audio;
class AudioWave;
class Renderer;
class Sqlite3Conector;
class RecordTable;
struct RecordData;

struct App
{

	static Window* pWin;
	static DIBSection* pDS;
	static Mouse* pMouse;
	static Audio* pAudio;
	static std::vector<AudioWave*> waves;
	static cdbpp::cdbpp* pCdb;
	static Sqlite3Conector* pDbConnector;
	static Renderer* pRenderer;
	static vec2 lookat;
	static vec2 eye;
	static wchar_t noticeBuffer[64];
	static GameProcess* pGameProcess;
	static Camera* pCamera;
	static RecordTable* pRecordTable;
	static RecordData record;

	static ParticleGroup *pParticleGroup;

	static Player* pPlayer;
	static std::list<Enemy*> enemies;
	static u32 life;
	static bool isStop;
	static u32 fps;
	static f64 lastCountedFps;
	static u32 lastCountedTime;
	static u32 lastCountedFrame;
	static bool renderScore;
	static bool renderLife;

	static f32 gakugaku;

	static bool setup();
	static bool teardown();
	static void run();
	static void updateMVP();
	static void render();
	static vec2 mousePos();
	static void addScore(u32 combo);
	static void renderText(vec2 pos, u32 color, const wchar_t* pText);
	static void setPlayWave(u32 index, bool doesPlay = true, bool doesLoop = false);

	static void stop();
	static void start();
	static void setFps(u32 fps);
	static u32 getFps();
	static void addLife();

	static void initRecord();
	static void saveRecord();

}; // struct App

