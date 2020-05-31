
#include "precompile.h"
#include "util.h"

#include "app.h"
#include "particle_group.h"
#include "particle.h"

void EmitConeParticle(
	u32 amount, vec3 center,
	f32 y_angle_center, f32 y_angle_range,
	f32 xz_angle_center, f32 xz_angle_range,
	f32 velo_center, f32 velo_range,
	u32 tick_center, u32 tick_range,
	vec3 acc_center, vec3 acc_range,
	u32 color )
{

	for (int i = 0; i < amount; i++) {
		vec3 acc = vec3(
			RandRangeMid(acc_center.x, acc_range.x),
			RandRangeMid(acc_center.y, acc_range.y),
			RandRangeMid(acc_center.z, acc_range.z));
		
		f32 velo_length = RandRangeMid(velo_center, velo_range);
		f32 y_angle = RandRangeMid(y_angle_center, y_angle_range);
		f32 xz_angle = RandRangeMid(xz_angle_center, xz_angle_range);
		
		f32 velo_y_length = sinf(xz_angle) * velo_length;
		f32 velo_xz_length = cosf(xz_angle) * velo_length;
		vec3 velo = vec3(cosf(y_angle) * velo_xz_length, velo_y_length, sinf(y_angle) * velo_xz_length);
		u32 tick = RandRangeMid(tick_center, tick_range);

		Particle* pParticle = App::pParticleGroup->pop();
		pParticle->onInit(center, velo, acc, color, tick, 2);
		App::pParticleGroup->push(pParticle);
	}
}


bool IsCollideSquare(vec2 c1, f32 l1, f32 a1, vec2 c2, f32 l2, f32 a2)
{
	// ˆø”‚Å‚¤‚¯‚é‚Ì‚Í³•ûŒ`‚Ì•Ó‚Ì’·‚³ / 2 ‚È‚Ì‚Å
	// SQRT(2)‚ð‚©‚¯‚Ä‚¨‚­‚±‚Æ‚Å’†S‚©‚ç’¸“_‚Ü‚Å‚Ì’·‚³‚É‚µ‚Ä‚¨‚­
	l1 *= SQRT2;
	l2 *= SQRT2;

	// æ‚É‰~‚Å‚´‚Á‚­‚è‚Æ”»’è‚µ‚Ä‚¨‚­
	if (glm::length(c2 - c1) > (l1 + l2)) {
		return false;
	}

	vec2 p1[4];
	for (int i = 0; i < 4; i++) {
		f32 a = a1 + PI * (0.25f + 0.5f * i);
		p1[i] = c1 + Pcs(l1, a);
	}
	vec2 p2[4];
	for (int i = 0; i < 4; i++) {
		f32 a = a2 + PI * (0.25f + 0.5f * i);
		p2[i] = c2 + Pcs(l2, a);
	}

	for (int i2 = 0; i2 < 4; i2++) {
		bool allInner = true;
		for (int i1_1 = 0; i1_1 < 4; i1_1++) {
			int i1_2 = (i1_1 + 1) % 4;
			vec2 v1 = p1[i1_2] - p1[i1_1];
			vec2 v2 = p2[i2]    - p1[i1_1];
			f32 cross = v1.x * v2.y - v1.y * v2.x;
			if (cross < 0.0f) {
				allInner = false;
				break;
			}
		}
		if (allInner) {
			return true;
		}
	}

	for (int i1 = 0; i1 < 4; i1++) {
		bool allInner = true;
		for (int i2_1 = 0; i2_1 < 4; i2_1++) {
			int i2_2 = (i2_1 + 1) % 4;
			vec2 v1 = p2[i2_2] - p2[i2_1];
			vec2 v2 = p1[i1]   - p2[i2_1];
			f32 cross = v1.x * v2.y - v1.y * v2.x;
			if (cross < 0.0f) {
				allInner = false;
				break;
			}
		}
		if (allInner) {
			return true;
		}
	}


	return false;
}
