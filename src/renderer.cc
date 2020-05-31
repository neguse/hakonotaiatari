
#include "precompile.h"
#include "renderer.h"

#include "obj.h"
#include "dib_section.h"
#include "app.h"

namespace {

	static const f32 CLIP_NEAR = -100.0f;
	static const f32 CLIP_FAR  = 100.0f;

	// related:
	// http://cs.wpunj.edu/~najarian/cs461/open-gl/H&B_code_expanded/clipCohSuth.c

	static const u32 CLIP_CODE_LEFT		= (1 << 0);
	static const u32 CLIP_CODE_RIGHT	= (1 << 1);
	static const u32 CLIP_CODE_TOP		= (1 << 2);
	static const u32 CLIP_CODE_BOTTOM	= (1 << 3);

	static inline bool IsInside(u32 a)
	{
		return !a;
	}

	static inline bool IsReject(u32 a, u32 b)
	{
		return (a & b);
	}

	static inline bool IsAccept(u32 a, u32 b)
	{
		return (!(a | b));
	}

	static inline u32 CalcClipCode(i32 x, i32 y, i32 l, i32 r, i32 t, i32 b)
	{
		CHECK(l <= r) << "l is lefter(lower) than r";
		CHECK(t <= b) << "t is upper(lower) than b";

		u32 code = 0;

		if (x < l)
			code |= CLIP_CODE_LEFT;
		if (r < x)
			code |= CLIP_CODE_RIGHT;
		if (y < t)
			code |= CLIP_CODE_TOP;
		if (b < y)
			code |= CLIP_CODE_BOTTOM;

		return code;
	}

	// クリップ処理をおこなう。引数はクリップされる。
	// 描画できるところが一個もなかったらfalseが返る。
	static bool ClipLine(i32& x1, i32& y1, i32& x2, i32& y2, u32 w, u32 h)
	{
		i32 l = 0;
		i32 r = w - 1;
		i32 t = 0;
		i32 b = h - 1;
		bool ret = false;

		for (;;) {
			u32 code1 = CalcClipCode(x1, y1, l, r, t, b);
			u32 code2 = CalcClipCode(x2, y2, l, r, t, b);

			if (IsAccept(code1, code2)) {
				ret = true;
				break;
			} else if (IsReject(code1, code2)) {
				break;
			} else {
				if (IsInside(code1)) {
					Swap(x1, x2);
					Swap(y1, y2);
					Swap(code1, code2);
				}

				f32 m;
				if (x1 != x2) {
					m = (f32)(y2 - y1) / (f32)(x2 - x1);
				} else {
					m = 1.0f;
				}

				if (code1 & CLIP_CODE_LEFT) {
					y1 += (i32)((l - x1) * m);
					x1 = l;
				} else if (code1 & CLIP_CODE_RIGHT) {
					y1 += (i32)((r - x1) * m);
					x1 = r;
				} else if (code1 & CLIP_CODE_BOTTOM) {
					if (x1 != x2) {
						x1 += (i32)((b - y1) / m);
					}
					y1 = b;
				} else if (code1 & CLIP_CODE_TOP) {
					if (x1 != x2) {
						x1 += (i32)((t - y1) / m);
					}
					y1 = t;
				}
			}
		}
		return ret;
	}

	static inline vec3 TransformPosition(const vec3& src, const mat4& mvp, const mat4& viewport)
	{
		vec4 ret = mvp * vec4(src, 1.0f);
		ret /= ret.w;
		ret = viewport * ret;
		return ret.swizzle(glm::X, glm::Y, glm::Z);
	}

	static vec3 RandomSwing( f32 maxRadius ) 
	{
		float a = 2.0f * PI * float(rand()) / RAND_MAX;
		float r = maxRadius * float(rand()) / RAND_MAX;
		vec2 pos = Pcs(r, a);
		return vec3(pos.x, pos.y, 0.f);
	}


} // namespace

void Renderer::renderLine( const vec3& p1, const vec3& p2, u32 color )
{
	RenderLine(m_pDS, m_mvp, m_viewport, p1, p2, color, m_gakugaku);
}

void Renderer::renderWire( const vec3* pVBuffer, u32 vcount, const u32* pIBuffer, u32 icount, u32 color )
{
	RenderWire(m_pDS, m_mvp, m_viewport, pVBuffer, icount, pIBuffer, icount, color, m_gakugaku);
}

void Renderer::renderObj( const OBJ_FILE* pObj, u32 color )
{
	RenderObj(m_pDS, m_mvp, m_viewport, pObj, color, m_gakugaku);
}

void Renderer::renderDot( vec3 pos, u32 color , u32 size)
{
	RenderDot(m_pDS, m_mvp, m_viewport, pos, color, m_gakugaku, size);
}

void RenderLine(DIBSection* pDS,
		const mat4& mvp, const mat4& viewport,
		const vec3& p1, const vec3& p2,
		u32 color, f32 gakugaku)
{
	vec3 transformed_v1 = TransformPosition(p1, mvp, viewport);
	transformed_v1 += RandomSwing(gakugaku);
	vec3 transformed_v2 = TransformPosition(p2, mvp, viewport);
	transformed_v2 += RandomSwing(gakugaku);

	if (IsInRange(transformed_v1.z, CLIP_NEAR, CLIP_FAR)
		&& IsInRange(transformed_v2.z, CLIP_NEAR, CLIP_FAR))
	{
		DrawLine(pDS,
			transformed_v1.x, transformed_v1.y,
			transformed_v2.x, transformed_v2.y,
			color);
	}
}


void RenderWire(DIBSection* pDS,
		const mat4& mvp, const mat4& viewport,
		const vec3* pVBuffer, u32 vcount,
		const u32* pIBuffer, u32 icount,
		u32 color, f32 gakugaku)
{
	std::vector<vec3> transformed(vcount);
	for (int i = 0; i < vcount; i++) {
		transformed[i] = TransformPosition(pVBuffer[i], mvp, viewport);
		transformed[i] += RandomSwing(gakugaku);
	}

	for (int i = 0; i < icount; i += 2) {
		u32 i1 = pIBuffer[i];
		u32 i2 = pIBuffer[i + 1];
		vec3& transformed_v1 = transformed[i1];
		vec3& transformed_v2 = transformed[i2];

		if (IsInRange(transformed_v1.z, CLIP_NEAR, CLIP_FAR)
			&& IsInRange(transformed_v2.z, CLIP_NEAR, CLIP_FAR))
		{
			DrawLine(pDS,
				transformed_v1.x, transformed_v1.y,
				transformed_v2.x, transformed_v2.y,
				color);
		}


	}
}

void RenderObj(DIBSection* pDS,
		const mat4& mvp, const mat4& viewport,
		const OBJ_FILE* pObj,
		u32 color, f32 gakugaku)
{
	std::vector<vec3> transformed(pObj->vertex_count);

	const float* vaddr = pObj->vertexAddr();
	u32 vcount = pObj->vertex_count;
	for (int i = 0; i < vcount; i++) {
		vec3 t;
		switch (pObj->vertex_type) {
			case VERTEX_TYPE_XY:
				t = vec3(vaddr[0], vaddr[1], 0.0f);
				vaddr += 2;
				break;
			case VERTEX_TYPE_XYZ:
				t = vec3(vaddr[0], vaddr[1], vaddr[2]);
				vaddr += 3;
				break;
			default:
				LOG(FATAL) << "not supported type " << pObj->vertex_type;
				break;
		}
		transformed[i] = TransformPosition(t, mvp, viewport);
		transformed[i] += RandomSwing(gakugaku);
	}

	u32 icount = pObj->index_count;
	for (int i = 0; i < icount; i += 2) {
		u32 i1 = pObj->indexAddr()[i];
		u32 i2 = pObj->indexAddr()[i + 1];
		const vec3& transformed_v1 = transformed[i1];
		const vec3& transformed_v2 = transformed[i2];

		if (IsInRange(transformed_v1.z, CLIP_NEAR, CLIP_FAR)
			&& IsInRange(transformed_v2.z, CLIP_NEAR, CLIP_FAR))
		{
			DrawLine(pDS,
				transformed_v1.x, transformed_v1.y,
				transformed_v2.x, transformed_v2.y,
				color);
		}
	}
}

void RenderDot(DIBSection* pDS,
		const mat4& mvp, const mat4& viewport,
		vec3 pos,
		u32 color,
		f32 gakugaku,
		u32 size)
{

	vec3 transformed = TransformPosition(pos, mvp, viewport);
	transformed += RandomSwing(gakugaku);

	if (IsInRange(transformed.z, CLIP_NEAR, CLIP_FAR)) {
		for (u32 dx = 0; dx < size; dx++) {
			for (u32 dy = 0; dy < size; dy++) {
				u32 x = transformed.x + dx - size/2;
				u32 y = transformed.y + dy - size/2;
				if (0 <= x && x < pDS->getWidth()
					&& 0 <= y&& y < pDS->getHeight())
				{
					pDS->setPixel(x, y, color);
				}

			}
		}
	}

}

void DrawLine(DIBSection* pDS, i32 x1, i32 y1, i32 x2, i32 y2, u32 color)
{
	u32 w = pDS->getWidth();
	u32 h = pDS->getHeight();

	if (!ClipLine(x1, y1, x2, y2, w, h)) {
		return;
	}

	if (x1 > x2) {
		Swap(x1, x2); Swap(y1, y2);
	}

	i32 dx = x2 - x1;
	i32 dy = y2 - y1;

	if (dy > 0) {

		if (dx > dy) {
			// かたむきがゆるい
			// xをインクリメントしていく

			i32 my = dy / 2;
			i32 py = y1;

			for (i32 px = x1; px <= x2; px++) {
				pDS->setPixel(px, py, color);
				my += dy;
				if (my > dx) {
					py++; my -= dx;
				}
			}
		} else {
			// かたむきがきゅう
			// yをインクリメントしていく
			i32 mx = dx / 2;
			i32 px = x1;

			for (i32 py = y1; py <= y2; py++) {
				pDS->setPixel(px, py, color);
				mx += dx;
				if (mx > dy) {
					px++; mx -= dy;
				}
			}
		}

	} else {

		if (dx > -dy) {
			// かたむきがゆるい
			// xをインクリメントしていく

			i32 my = dy / 2;
			i32 py = y1;

			for (i32 px = x1; px <= x2; px++) {
				pDS->setPixel(px, py, color);
				my += -dy;
				if (my > dx) {
					py--; my -= dx;
				}
			}
		} else {
			// かたむきがきゅう
			// yをデクリメントしていく
			i32 mx = dx / 2;
			i32 px = x1;

			for (i32 py = y1; py >= y2; py--) {
				pDS->setPixel(px, py, color);
				mx += dx;
				if (mx > -dy) {
					px++; mx -= -dy;
				}
			}
		}
	}
}

void Fill( DIBSection* pDS, u32 color)
{
	u32 pixels = pDS->getWidth() * pDS->getHeight();
	u32* pBuffer = pDS->getBufferAddress();

	// TODO: できれば最適化する
	for (u32 i = 0; i < pixels; i++) {
		pBuffer[i] = color;
	}
}

void FillRect(DIBSection* pDS, i32 x, i32 y, i32 w, i32 h, u32 color)
{
	u32* pBuffer = pDS->getBufferAddress();
	u32 width = pDS->getWidth();

	// TODO: できれば最適化する
	for (int px = x; px < x + w; px++) {
		for (int py = y; py < y + h; py++) {
			pBuffer[width * y + x] = color;
		}
	}
}

