
#pragma once

class Window;
class DIBSection;
struct OBJ_FILE;

class Renderer {

private:

	Window* m_pWindow;
	DIBSection* m_pDS;
	f32 m_gakugaku;
	mat4 m_model;
	mat4 m_view;
	mat4 m_proj;
	mat4 m_mvp;
	mat4 m_viewport;

public:
	Renderer(Window* pWindow, DIBSection* pDS)
		: m_pWindow(pWindow)
		, m_pDS(pDS)
		, m_gakugaku(0.0f)
	{
	}

	~Renderer()
	{
	}

	void setModel(const mat4& model)
	{
		m_model = model;
	}
	const mat4& getModel() const
	{
		return m_model;
	}
	void setView(const mat4& view)
	{
		m_view = view;
	}
	const mat4& getView() const
	{
		return m_view;
	}

	void setProjection(const mat4& proj)
	{
		m_proj = proj;
	}
	const mat4& getProjection() const
	{
		return m_proj;
	}

	void setViewport(const mat4& viewport)
	{
		m_viewport = viewport;
	}
	const mat4& getViewport() const
	{
		return m_viewport;
	}

	void updateMVP()
	{
		m_mvp = m_proj * m_view * m_model;
	}
	void setMVP(const mat4& mvp)
	{
		m_mvp = mvp;
	}
	const mat4& getMVP() const
	{
		return m_mvp;
	}

	f32 getGakugaku() const
	{
		return m_gakugaku;
	}

	void setGakugaku(f32 gakugaku)
	{
		m_gakugaku = gakugaku;
	}

	void renderLine(
		const vec3& p1, const vec3& p2,
		u32 color);

	void renderWire(
		const vec3* pVBuffer, u32 vcount,
		const u32* pIBuffer, u32 icount,
		u32 color);

	void renderObj(
		const OBJ_FILE* pObj,
		u32 color);

	void renderDot(
		vec3 pos,
		u32 color,
		u32 size);

}; // Renderer

extern void RenderLine(DIBSection* pDS,
	const mat4& mvp, const mat4& viewport,
	const vec3& p1, const vec3& p2,
	u32 color, f32 gakugaku);

extern void RenderWire(DIBSection* pDS,
	const mat4& mvp, const mat4& viewport,
	const vec3* pVBuffer, u32 vcount,
	const u32* pIBuffer, u32 icount,
	u32 color, f32 gakugaku);

extern void RenderObj(DIBSection* pDS,
	const mat4& mvp, const mat4& viewport,
	const OBJ_FILE* pObj,
	u32 color, f32 gakugaku);

extern void RenderDot(DIBSection* pDS,
	const mat4& mvp, const mat4& viewport,
	vec3 pos,
	u32 color, f32 gakugaku, u32 size);

extern void DrawLine(DIBSection* pDS,
	i32 x1, i32 y1, i32 x2, i32 y2, u32 color);

extern void Fill(DIBSection* pDS, u32 color);

extern void FillRect(DIBSection* pDS,
	i32 x, i32 y, i32 w, i32 h, u32 color);
