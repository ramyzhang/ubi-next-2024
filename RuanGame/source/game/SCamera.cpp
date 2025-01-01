#include "stdafx.h"

#include "SCamera.h"

void SCamera::Init() {
	SRenderer::Instance().camera.position = Vector3(0, 0, 0);
	SRenderer::Instance().camera.look = Vector3(0, 0, 1);
	SRenderer::Instance().camera.yaw = 0;
};

void SCamera::Update(const float deltaTime) {
	// Toggle wireframe view
	if (App::IsKeyPressed('Z')) {
		SRenderer::Instance().wireframe_view = true;
	}
	else {
		SRenderer::Instance().wireframe_view = false;
	}

	float speed = 0.03f;
	float yaw_speed = 0.005f;

	Camera newCamera = SRenderer::Instance().camera;

	Vector3 forward = newCamera.look * speed * deltaTime;
	Vector3 right = Vector3(0, 1, 0).cross(forward) * speed * deltaTime;

	if (App::GetController().GetLeftThumbStickX() > 0.5f)
	{
		newCamera.position.add(right);
	}
	if (App::GetController().GetLeftThumbStickX() < -0.5f)
	{
		newCamera.position.subtract(right);
	}
	if (App::GetController().GetLeftThumbStickY() > 0.5f)
	{
		newCamera.position.y += speed * deltaTime;
	}
	if (App::GetController().GetLeftThumbStickY() < -0.5f)
	{
		newCamera.position.y -= speed * deltaTime;
	}

	if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_UP, false))
	{
		newCamera.position.add(forward);
	}
	if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_DOWN, false))
	{
		newCamera.position.subtract(forward);
	}
	if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_LEFT, false))
	{
		newCamera.yaw -= yaw_speed * deltaTime;
	}
	if (App::GetController().CheckButton(XINPUT_GAMEPAD_DPAD_RIGHT, false))
	{
		newCamera.yaw += yaw_speed * deltaTime;
	}

	SRenderer::Instance().camera = newCamera;
}