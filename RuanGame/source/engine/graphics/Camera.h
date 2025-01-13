#pragma once

#include "math/Matrix4x4.h"
#include "App/main.h"
#include "App/app.h"

class Camera {
public:
	void Init();
	void HandleInputs(const float deltaTime);

	Vector3		position;
	Vector3		look;
	float		yaw; // in radians

	Matrix4x4	projection;
	Matrix4x4	view;

private:
	void UpdateViewMatrix();

	float	m_near = 0.1f;
	float	m_far = 1000;
	float	m_fov = 90;
	float	m_aspectratio = (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH;
	float	m_fovrad = 1 / tanf(m_fov * 0.5f / 180.0f * (float)M_PI);
};