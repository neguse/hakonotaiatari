
#include "precompile.h"
#include "app.h"

#include "window.h"
#include "dib_section.h"
#include "mouse.h"
#include "game_process.h"
#include "audio.h"
#include "camera.h"
#include "renderer.h"
#include "title_state.h"
#include "test_state.h"
#include "particle_group.h"
#include "player.h"
#include "enemy.h"
#include "record.h"

Window* App::pWin;
DIBSection* App::pDS;
Mouse* App::pMouse;
cdbpp::cdbpp* App::pCdb;
Sqlite3Conector* App::pDbConnector;
Renderer* App::pRenderer;
Audio* App::pAudio;
std::vector<AudioWave*> App::waves;
vec2 App::lookat;
vec2 App::eye;
GameProcess* App::pGameProcess;
Camera* App::pCamera;
ParticleGroup* App::pParticleGroup;
Player* App::pPlayer;
std::list<Enemy*> App::enemies;
wchar_t App::noticeBuffer[64];
bool App::isStop;
u32 App::fps;
f64 App::lastCountedFps;
u32 App::lastCountedFrame;
u32 App::lastCountedTime;
bool App::renderScore;
bool App::renderLife;

RecordTable* App::pRecordTable;
RecordData App::record;

extern "C" {
	extern char binary_data_dat_start[];
	extern char binary_data_dat_end[];
	extern char binary_data_dat_size[];
}

static mat4 GetViewport(u32 x, u32 y, u32 w, u32 h)
{
	return mat4(
			float(w) * 0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -float(h) * 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			float(w) * 0.5, float(h) * 0.5, 0.0f, 1.0f);
}

static void renderField()
{
	mat4 model = glm::scale(mat4(1.0f), vec3(FIELD_L / 40.0f)) * glm::mat4_cast(quat(vec3(PI * 0.5f, 0.f, 0.f)));
	App::pRenderer->setModel(model);
	App::pRenderer->updateMVP();

	OBJ_FILE* pObj = (OBJ_FILE*)App::pCdb->get("field.obj", 9, NULL);

	App::pRenderer->renderObj(pObj, 0xff606060);
}

bool App::setup()
{
	LOG(INFO) << "App::setup()";
	
	// XAudio2用
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	{
		pDbConnector = new Sqlite3Conector();
		std::string path = getenv("appdata");
		path += APP_DB_PATH;
		Retval ret = pDbConnector->initialize(path.c_str());
		if (ret.isFail()) {
			LOG(ERROR) << "DBファイルのオープンに失敗しました";
			goto FAIL;
		}
	}
	{
		pRecordTable = new RecordTable();
		Retval ret = pRecordTable->initialize(pDbConnector);
		if (ret.isFail()) {
			LOG(ERROR) << "テーブルの生成に失敗しました";
			goto FAIL;
		}

	}

	{
		pWin = new Window();
		Retval ret = pWin->initialize(WIN_W, WIN_H);
		if (ret.isFail()) {
			LOG(ERROR) << "ウィンドウの生成に失敗しました";
			goto FAIL;
		}
	}

	{
		pDS = new DIBSection();
		Retval ret = pDS->initialize(pWin->getHandle(), RT_W, RT_H);
		if (ret.isFail()) {
			LOG(ERROR) << "DIBSectionの生成に失敗しました";
			goto FAIL;
		}

		pRenderer = new Renderer(pWin, pDS);
	}
	
	{
		pMouse = new Mouse(pWin);
	}

	pCdb = new cdbpp::cdbpp(binary_data_dat_start, (size_t)binary_data_dat_size, false);

	{
		pAudio = new Audio();
		Retval ret = pAudio->initialize();
		if (ret.isFail()) {
			LOG(ERROR) << "Audioの生成に失敗しました";
			goto FAIL;
		}

		static const char* WAVE_FILE_NAME[WAVE_FILE_MAX] = {
			"hakotai.wav",
			"ne4.wav",
			"sakura.wav",
			"tt.wav",
			"hit1.wav",
			"suberi.wav",
			"hit4.wav",
			"fire.wav",
			"fall.wav",
			"powerfull.wav",
			"result.wav",
			"result_high.wav",
			"kaki.wav",
			"revirth.wav",
			"suberie.wav",

		};
		for (int i = 0; i < WAVE_FILE_MAX; i++) {
			const char* pFileName = WAVE_FILE_NAME[i];
			size_t waveSize = 0;
			const void* pWaveData = pCdb->get(pFileName, strlen(pFileName), &waveSize);
			if (pWaveData == NULL) {
				LOG(ERROR) << "ファイル「" << pFileName <<  "」の読み込みに失敗しました";
				goto FAIL;
			}
			AudioWave* pWave = pAudio->createWave(pWaveData, waveSize);
			if (pWave == NULL) {
				LOG(ERROR) << "createWaveに失敗しました";
				goto FAIL;
			}
			waves.push_back(pWave);
		}
	}

	pGameProcess = new GameProcess();
	pParticleGroup = new ParticleGroup();

	pCamera = new Camera();
	mat4 viewport = GetViewport(0, 0, pDS->getWidth(), pDS->getHeight());
	pRenderer->setViewport(viewport);

	pPlayer = new Player();

	App::fps = FPS;

	::timeBeginPeriod(1);

#if ENABLE_VARIABLE
	VariableHolder::init();
#endif

	pCamera->setBehind(CAM_BEHIND_HIGH, CAM_BEHIND_BACK);
	pCamera->update();

	App::initRecord();

	return true;

FAIL:

	LOG(ERROR) << "アプリケーションの初期化に失敗しました";
	teardown();
	return false;
}

bool App::teardown()
{
	::timeEndPeriod(1);

	SAFE_DELETE(pPlayer);
	SAFE_DELETE(pParticleGroup);

	SAFE_DELETE(pCamera);
	SAFE_DELETE(pCdb);

	SAFE_DELETE(pRenderer);

	while (!waves.empty()) {
		AudioWave* pWave = waves.back();
		waves.pop_back();
		pWave->finalize();
		delete pWave;
	}
	
	if (pAudio != NULL) {
		Retval ret = pAudio->finalize();
		if (ret.isFail()) {
			LOG(ERROR) << "Audioの破棄に失敗しました";
		}
		SAFE_DELETE(pAudio);
	}

	SAFE_DELETE(pMouse);

	if (pDS != NULL) {
		Retval ret = pDS->finalize();
		if (ret.isFail()) {
			LOG(ERROR) << "DIBSectionの破棄に失敗しました";
		}
		SAFE_DELETE(pDS);
	}
	if (pWin != NULL) {
		Retval ret = pWin->finalize();
		if (ret.isFail()) {
			LOG(ERROR) << "Windowの破棄に失敗しました";
		}
		SAFE_DELETE(pWin);
	}

	if (pRecordTable != NULL) {
		Retval ret = pRecordTable->finalize();
		if (ret.isFail()) {
			LOG(ERROR) << "テーブルインスタンスの破棄に失敗しました";
		}
		SAFE_DELETE(pRecordTable);
	}
	if (pDbConnector != NULL) {
		Retval ret = pDbConnector->finalize();
		if (ret.isFail()) {
			LOG(ERROR) << "DBコネクタの破棄に失敗しました";
		}
		SAFE_DELETE(pDbConnector);
	}

	CoUninitialize();

	return true;
}

void App::run()
{
	pGameProcess->trans(new TitleState());
	//pGameProcess->trans(new TestState());

	u32 frames = 0;
	f32 dt = DELTA_T; // とりあえず固定フレームで

	u32 t1 = ::timeGetTime();
	u32 t2 = t1;

	App::lastCountedFps = 0.f;
	App::lastCountedFrame = 0;
	App::lastCountedTime = t1;
	u32 renderedCount = 0;
	u32 skipCount = 0;

	while (!pWin->isQuit()) {

		frames++;

		vec2 windowSize = pWin->getWindowSize();
		vec2 cursorPos = pMouse->getCursorPos();
		vec2 ds_size = vec2(pDS->getWidth(), pDS->getHeight());

		vec2 scale_ds_to_window = windowSize / ds_size;
		float scale = glm::min(scale_ds_to_window.x, scale_ds_to_window.y);
		vec2 scaled_ds_size = ds_size * scale;
		vec2 ds_offset = (windowSize - scaled_ds_size) / 2.f;


		u32 willTime = lastCountedTime + (frames - lastCountedFrame) * 1000 / FPS;
		t1 = t2;
		bool updated = false;

		if ((t1 - lastCountedTime) >= 2000) {
			lastCountedFps = 1000 * renderedCount / (t1 - lastCountedTime);
			lastCountedFrame = frames;
			lastCountedTime = t1;
			renderedCount = 0;
			updated = true;
		}

		pMouse->setWindowSize(ds_offset, scaled_ds_size);
		pWin->doEvents();
#if ENABLE_VARIABLE
		VariableHolder::update();
#endif


		// フレーム処理
		if (!App::isStop && App::pWin->isActive()) {
			pMouse->update();
			pGameProcess->onUpdate(dt);
		}

		// フレーム描画
		if ((::timeGetTime() < willTime) || updated) {

			pParticleGroup->onUpdate(skipCount + 1);
			skipCount = 0;

			renderedCount++;
			HWND hWindow = pWin->getHandle();
			HDC hWinDC = GetDC(hWindow);

			Fill(pDS, 0x00000000);

			// update matrix
			mat4 projection = glm::perspective(45.0f, ds_size.x / ds_size.y, 1.0f, 1000.f);
			pRenderer->setProjection(projection);
			pRenderer->setView(pCamera->view());

			render();

			StretchBlt(hWinDC,
					ds_offset.x, ds_offset.y, scaled_ds_size.x, scaled_ds_size.y,
					pDS->getDDB(),
					0, 0, ds_size.x, ds_size.y,
					SRCCOPY);

			ReleaseDC(hWindow, hWinDC);
		} else {
			skipCount++;
		}

		t2 = ::timeGetTime();

		if (t2 < willTime) {
			// まだあわてる時間ではないので、待つ
			i32 sleep_tick = willTime - t2;
			Sleep(sleep_tick);
		}

		t2 = ::timeGetTime();
	}
}

void App::render()
{
	renderField();
	pPlayer->onRender();
	for (std::list<Enemy*>::iterator it = App::enemies.begin(); it != App::enemies.end(); it++) {
		(*it)->onRender();
	}
	pParticleGroup->render();

	wchar_t buf[1024];

	if (renderScore) {
		swprintf(buf, L"score:%d", record.score);
		renderText(vec2(10, 19), 0xffffff, buf);
	}
	if (renderLife) {
		swprintf(buf, L"life:");
		int i = 0;
		for (i = 0; i < glm::min(pPlayer->life(), 8); i++) {
			buf[5 + i] = L'命';
		}
		buf[5 + i] = '\0';
		renderText(vec2(0, 19), 0xffffff, buf);
	}

	renderText(vec2(3, 10), 0xffffffff, App::noticeBuffer);

	pGameProcess->onRender();

// 
// 	swprintf(buf, L"particles:%d", pParticleGroup->getCount());
// 	renderText(vec2(0, 17), 0xffffff, buf);
// 
	swprintf(buf, L"FPS:%2.2f", lastCountedFps);
	renderText(vec2(0, 18), 0xffffff, buf);
}

vec2 App::mousePos()
{
	mat4 revVP = glm::inverse(pRenderer->getProjection() * pRenderer->getView());
	vec2 cursorPos = pMouse->getCursorPos();
	vec4 cursorVec(cursorPos.x, cursorPos.y, 0.0f, 1.0f);
	vec4 cursorVec1(cursorPos.x, cursorPos.y, -1.0f, 1.0f);
	cursorVec = (revVP * cursorVec);
	cursorVec1 = (revVP * cursorVec1);
	cursorVec /= cursorVec.w;
	cursorVec1 /= cursorVec1.w;
	vec4 rayDir1 = cursorVec - cursorVec1;
	vec4 pos = cursorVec - (cursorVec.y / rayDir1.y) * rayDir1;
	CHECK(!_isnan(pos.x)) << "pos.x is NAN!";
	CHECK(!_isnan(pos.z)) << "pos.y is NAN!";
	
	return glm::clamp(vec2(pos.x, pos.z), vec2(-1000.0f, -1000.0f), vec2(1000.0f, 1000.0f));
}

void App::addScore( u32 combo )
{
	record.score += combo;
}

void App::renderText(vec2 pos, u32 color, const wchar_t* pText)
{
	while (*pText) {
		wchar_t code = *pText;
		char font_name[32];
		sprintf(font_name, "KST32B.TXT_%04x", code);

		const OBJ_FILE* pObj = reinterpret_cast<const OBJ_FILE*>(App::pCdb->get(font_name, strlen(font_name), NULL));
		if (pObj != NULL) {

			mat4 mvp = glm::scale(
				glm::translate(
					mat4(1.0f), vec3(-0.95f + pos.x * 0.10, -1.05 + pos.y * 0.10, 0.0)),
				vec3(0.0030f));
			App::pRenderer->setMVP(mvp);
			App::pRenderer->renderObj(pObj, color);
		}
		if (code <= 0xff) {
			pos.x += 0.5f;
		} else {
			pos.x += 1.0f;
		}
		pText++;
	}
}

void App::setPlayWave( u32 index, bool doesPlay, bool doesLoop )
{
	if (doesPlay) {
		waves[index]->stop();
		waves[index]->play(doesLoop);
	} else {
		waves[index]->stop();
	}
}


void App::stop()
{
	App::isStop = true;
}

void App::start()
{
	App::isStop = false;
}

void App::setFps( u32 fps )
{
	App::fps = fps;
}

u32 App::getFps()
{
	return App::fps;
}

void App::addLife()
{
	if (pPlayer != NULL) {
		pPlayer->setLife(pPlayer->life() + 1);
	}
}

void App::initRecord()
{
	record.id = -1;
	record.score = 0;
}

void App::saveRecord()
{
	bool ret = pRecordTable->insertRecord(record);
	if (!ret) {
		LOG(ERROR) << "レコードの保存に失敗しました";
	}
}
