
#pragma once

class Camera
{
private:
	vec3 m_eye;
	vec3 m_lookat;
	f32 m_behind_high;
	f32 m_behind_back;
	f32 m_rot;
	bool m_doesRotate;

public:

	Camera()
		: m_doesRotate(false)
		, m_rot(0.f)
	{

	}

	void setEnableRotate(bool on)
	{
		m_doesRotate = on;
		m_rot = PI * 1.5f;
	}

	void update()
	{

		if (m_doesRotate) {
			m_rot = SubRad(m_rot, -CAM_ROT_SPEED);
			vec3 diff = vec3(cosf(m_rot) * m_behind_back, m_behind_high, sinf(m_rot) * m_behind_back);
			m_eye = m_eye + (m_lookat + diff - m_eye) * CAM_BEHIND_COEFF;
		} else {
			m_eye = m_eye + ((m_lookat + vec3(0.0f, m_behind_high, -m_behind_back)) - m_eye) * CAM_BEHIND_COEFF;
		}
	}

	void setLookat(vec3 lookat)
	{
		m_lookat = lookat;
	}

	const vec3& getLookat() const
	{
		return m_lookat;
	}

	void setEye(vec3 eye)
	{
		m_eye = eye;
	}
	const vec3& getEye() const
	{
		return m_eye;
	}

	void setBehind(f32 high, f32 back)
	{
		m_behind_high = high;
		m_behind_back = back;
	}

	mat4 view()
	{
		return glm::lookAt(
				m_eye,
				m_lookat,
				vec3(0.0f, 1.0f, 0.0f));
	}

}; // class Camera

