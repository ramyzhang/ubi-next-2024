#include "stdafx.h"

#include "SCamera.h"

void SCamera::Init() {
	position = Vector3(0, 0, 0);
	look = Vector3(0, 0, 1);
	yaw = 0;
	pitch = 0;

	projection[0][0] = m_aspectratio * m_fovrad;
	projection[1][1] = m_fovrad;
	projection[2][2] = m_far / (m_far - m_near);
	projection[2][3] = 1;
	projection[3][2] = (-m_far * m_near) / (m_far - m_near);

	m_prev_mouse_x = 0.0f;
	m_prev_mouse_y = 0.0f;
	App::GetMousePos(m_prev_mouse_x, m_prev_mouse_y);
};

void SCamera::HandleInputs(const float deltaTime) {
	switch (mode) {
		case SCamera::FREE:
			FreeCameraUpdate(deltaTime);
			break;
		case SCamera::CLAMPED:
			ClampedCameraUpdate(deltaTime);
			break;
		case SCamera::FIXEDTARGET:
			break;
		default:
			break;
	}
	
	UpdateViewMatrix();
}

void SCamera::UpdateViewMatrix() {
	Vector3 up = Vector3(0, 1, 0);
	Vector3 target = Vector3(0, 0, 1);
	look = Matrix4x4().rotate(Vector3(pitch, yaw, 0)) * target;
	target = position + look;

	view = Matrix4x4().pointAt(position, target, up).dirtyInvert();
}

void SCamera::FreeCameraUpdate(const float deltaTime) {
	float speed = 0.03f;

	Vector3 forward = look * speed * deltaTime;
	Vector3 right = Vector3(0, 1, 0).cross(forward) * speed * deltaTime;

	// move camera
	if (App::GetController().GetLeftThumbStickX() > 0.5f)
	{
		position.add(right);
	}
	if (App::GetController().GetLeftThumbStickX() < -0.5f)
	{
		position.subtract(right);
	}
	if (App::GetController().GetLeftThumbStickY() > 0.5f)
	{
		position.add(forward);
	}
	if (App::GetController().GetLeftThumbStickY() < -0.5f)
	{
		position.subtract(forward);
	}

	// rotate camera with mouse
	float curr_mouse_x, curr_mouse_y;
	App::GetMousePos(curr_mouse_x, curr_mouse_y);

	float delta_x = curr_mouse_x - m_prev_mouse_x;
	float delta_y = curr_mouse_y - m_prev_mouse_y;

	// update camera rotation
	float target_yaw = yaw + delta_x * m_mouse_sens * deltaTime;
	float target_pitch = pitch - delta_y * m_mouse_sens * deltaTime;
	float new_yaw = Lerp(yaw, target_yaw, m_mouse_damping);
	float new_pitch = Lerp(pitch, target_pitch, m_mouse_damping);

	// clamp pitch to prevent camera flipping
	float clamp_l = - 70.0f * (float)M_PI / 180.0f;
	float clamp_r = 70.0f * (float)M_PI / 180.0f;
	pitch = std::clamp(new_pitch, clamp_l, clamp_r);
	yaw = new_yaw;

	m_prev_mouse_x = curr_mouse_x;
	m_prev_mouse_y = curr_mouse_y;
}

void SCamera::ClampedCameraUpdate(const float deltaTime) {
	Vector3 up = Vector3(0, 1, 0);
	Vector3 right = Vector3(0, 1, 0).cross(look);

	float curr_mouse_x, curr_mouse_y;
	App::GetMousePos(curr_mouse_x, curr_mouse_y);

	float delta_x = curr_mouse_x - m_prev_mouse_x;
	float delta_y = curr_mouse_y - m_prev_mouse_y;

	// update camera position
	Vector3 target = position + right * (delta_x * m_mouse_sens * deltaTime);
	target = target - up * (delta_y * m_mouse_sens * deltaTime);
	Vector3 new_pos = Lerp(position, target, m_mouse_damping);

	// clamp
	position.x = std::clamp(new_pos.x, position.x - 20.0f, position.x + 20.0f);
	position.y = std::clamp(new_pos.y, position.y - 20.0f, position.y + 20.0f);
	position.z = std::clamp(new_pos.z, position.z - 20.0f, position.z + 20.0f);

	m_prev_mouse_x = curr_mouse_x;
	m_prev_mouse_y = curr_mouse_y;
}
