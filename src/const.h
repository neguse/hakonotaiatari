
#pragma once

#define APP_DIR "\\negusema\\hakotai\\"
#define APP_LOG_DIR (APP_DIR "logs\\")
#define APP_DB_PATH (APP_DIR "data.db")
#define ENABLE_VARIABLE 0

static const f32 PI = 3.1415926535897932384626433832795f;
static const f32 SQRT2 = 1.4142135623730950488f;
static const u32 FPS = 60;
static const f32 DELTA_T = (1.0f / FPS);

static const u32 WIN_W = 240;
static const u32 WIN_H = 240;

static const u32 RT_W = 240;
static const u32 RT_H = 240;

static const u32 FIELD_L = 300;
static const f32 FIELD_Lf = 300;

static const u32 C_TYPE_PLAYER			= 0x00000001;
static const u32 C_TYPE_ENEMY_MASK		= 0x10000000;
static const u32 C_TYPE_NORMAL_ENEMY	= (C_TYPE_ENEMY_MASK | 0x01);
static const u32 C_TYPE_DASH_ENEMY		= (C_TYPE_ENEMY_MASK | 0x02);
//static const u32 C_TYPE_NORMAL_ENEMY	= (C_TYPE_ENEMY_MASK | 0x01);

static const u32 C_COL_ST_NONE		= 0;
static const u32 C_COL_ST_NORMAL	= 1;
static const u32 C_COL_ST_DASH		= 2;
static const u32 C_COL_ST_FUTTOBI	= 3;

static const u32 P_ST_MUTEKI	= 1;
static const u32 P_ST_NORMAL	= 2;
static const u32 P_ST_DASH		= 3;
static const u32 P_ST_FUTTOBI	= 4;
static const u32 P_ST_FUTTOBI_NOMUTEKI	= 5;
static const u32 P_ST_FADEOUT	= 6;
static const u32 P_ST_DEAD		= 7;

static const u32 E_ST_APPEAR	= 11;
static const u32 E_ST_MUTEKI	= 12;
static const u32 E_ST_NORMAL	= 13;
static const u32 E_ST_DASH		= 14;
static const u32 E_ST_FUTTOBI	= 15;
static const u32 E_ST_FADEOUT	= 16;
static const u32 E_ST_DEAD		= 17;

enum WaveIndex {
	WAVE_TITLE_INDEX,
	WAVE_BGM1_INDEX,
	WAVE_BGM2_INDEX,
	WAVE_BGM3_INDEX,
	WAVE_HIT1_INDEX,
	WAVE_SUBERI_INDEX,
	WAVE_HIT4_INDEX,
	WAVE_FIRE_INDEX,
	WAVE_FALL_INDEX,
	WAVE_POWERFULL_INDEX,
	WAVE_RESULT_INDEX,
	WAVE_RESULT_HIGH_INDEX,
	WAVE_NAGU_INDEX,
	WAVE_REVIRTH_INDEX,
	WAVE_SUBERIE_INDEX,
	WAVE_FILE_MAX,
};
