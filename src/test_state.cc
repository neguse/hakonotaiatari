
#include "precompile.h"
#include "test_state.h"

#include "app.h"

void TestState::onInit()
{
	LOG(INFO) << "TestState::onInit()";

	swprintf(App::noticeBuffer, L"左右クリックで回転オンオフ");

	// App::setPlayWave(WAVE_BGM_INDEX, true, true);

}

void TestState::onUpdate(f32 dt)
{
	static f32 angle = 0.f;
	static f32 camera_angle = 0.f;
	static f32 camera_xz_angle = 0.f;
	static bool does_rotate = false;
	static bool does_rotate_camera = false;

	if (does_rotate) {
		angle += dt;
	}
	if (does_rotate_camera) {
		camera_angle += dt;
		camera_xz_angle += dt;
	}

	if (App::pMouse->getLButtonDown()) {
		does_rotate = !does_rotate;
	}
// 	if (App::pMouse->getButtonPress(1)) {
// 		does_rotate_camera = !does_rotate_camera;
// 	}

	f32 CAMERA_D = 300.f;

	EmitConeParticle(80,
		vec3(),
		angle, 0.8f,
		PI / 2.f, 0.8f,
		100.f, 50.f,
		250, 50,
		vec3(0.f, -100.f, 0.f), vec3(5.0f, 0.f, 5.0f),
		0xffffffff);
	App::pCamera->setLookat(vec3());
	App::pCamera->setEye(vec3(cosf(camera_angle) * CAMERA_D, 50.f * cos(camera_xz_angle) + 100.f, sinf(camera_angle) * CAMERA_D));

	// App::pRenderer->setGakugaku(0.5f);
}

IState* TestState::nextState()
{
	return NULL;
}

