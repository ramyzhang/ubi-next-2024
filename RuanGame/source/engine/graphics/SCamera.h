#pragma once

#include <algorithm>
#include "math/Matrix4x4.h"
#include "math/Utils.h"
#include "App/main.h"
#include "App/app.h"
#include "core/System.h"

class SCamera : public System<SCamera> {
public:
	void Init();
	void HandleInputs(const float deltaTime);

	enum Mode {
		FREE,
		CLAMPED,
		FIXEDTARGET
	};

	Vector3		position;
	Vector3		look;
	float		yaw; // in radians
	float		pitch; // in radians

	Matrix4x4	projection;
	Matrix4x4	view;

	Mode		mode = FREE;

private:
	void UpdateViewMatrix();

	float	m_near = 0.1f;
	float	m_far = 1000;
	float	m_fov = 50;
	float	m_aspectratio = (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH;
	float	m_fovrad = 1 / tanf(m_fov * 0.5f / 180.0f * (float)M_PI);

	void FreeCameraUpdate(const float deltaTime);
	void ClampedCameraUpdate(const float deltaTime);

	float	m_prev_mouse_x = 0.0f;
	float	m_prev_mouse_y = 0.0f;
	float	m_mouse_sens = 0.0005f;

	float	m_mouse_damping = 0.1f;
};