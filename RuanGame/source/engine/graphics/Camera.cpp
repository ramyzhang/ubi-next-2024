#include "stdafx.h"

#include "Camera.h"

void Camera::Init() {
	position = Vector3(0, 0, 0);
	look = Vector3(0, 0, 1);
	yaw = 0;

	projection[0][0] = m_aspectratio * m_fovrad;
	projection[1][1] = m_fovrad;
	projection[2][2] = m_far / (m_far - m_near);
	projection[2][3] = 1;
	projection[3][2] = (-m_far * m_near) / (m_far - m_near);
};

void Camera::HandleInputs(const float deltaTime) {
	float speed = 0.03f;
	float yaw_speed = 0.005f;

	Vector3 forward = look * speed * deltaTime;
	Vector3 right = Vector3(0, 1, 0).cross(forward) * speed * deltaTime;

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
		position.y += speed * deltaTime;
	}
	if (App::GetController().GetLeftThumbStickY() < -0.5f)
	{
		position.y -= speed * deltaTime;
	}

	if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_UP, false))
	{
		position.add(forward);
	}
	if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_DOWN, false))
	{
		position.subtract(forward);
	}
	if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_LEFT, false))
	{
		yaw -= yaw_speed * deltaTime;
	}
	if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_RIGHT, false))
	{
		yaw += yaw_speed * deltaTime;
	}

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix() {
	Vector3 up = Vector3(0, 1, 0);
	Vector3 target = Vector3(0, 0, 1);
	look = Matrix4x4().rotate(Vector3(0, yaw, 0)) * target;
	target = position + look;

	view = Matrix4x4().pointAt(position, target, up).dirtyInvert();
}

