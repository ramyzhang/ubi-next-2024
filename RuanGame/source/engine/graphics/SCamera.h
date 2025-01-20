#pragma once

#include <algorithm>
#include "math/Matrix4x4.h"
#include "math/Utils.h"
#include "App/main.h"
#include "App/app.h"
#include "core/System.h"
#include "engine/ecs/SEntityManager.h"

// this system should mostly only be managed by the Renderer
// but i'm going to let other systems call it, since it can be involved in gameplay logic
class SCamera : public System<SCamera> {
public:
	void Init();
	void HandleInputs(const float deltaTime);

	enum Mode {
		FREE,
		CLAMPED,
		FIXEDTARGET,
		FIXEDTARGETROTATE,
	};

	Vector3		position;
	Vector3		look;
	float		yaw; // in radians
	float		pitch; // in radians

	Matrix4x4	projection; // prevents us from recalculating over and over in renderer
	Matrix4x4	view;
	Matrix4x4	view_inverse;

	Mode		mode = FREE;

	// it's crucial to set these in scene inits if we want to use the FIXEDTARGET mode
	void		SetTarget(const EntityID& _target) { m_target = _target; };
	void		SetTargetDistance(const Vector3& _rel_pos) { m_relative_pos = _rel_pos; };

private:
	void		UpdateViewMatrix();

	float		m_near = 0.1f;
	float		m_far = 900;
	float		m_fov = 50;
	float		m_aspectratio = (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH;
	float		m_fovrad = 1 / tanf(m_fov * 0.5f / 180.0f * (float)M_PI);

	// update functions for each camera mode
	void		FreeCameraUpdate(const float deltaTime);
	void		ClampedCameraUpdate(const float deltaTime);
	void		FixedTargetCameraUpdate(const float deltaTime);
	void		FixedTargetRotateCameraUpdate(const float deltaTime);

	float		m_prev_mouse_x = 0.0f;
	float		m_prev_mouse_y = 0.0f;
	float		m_mouse_sens = 0.0005f;
	float		m_mouse_damping = 0.1f;

	EntityID	m_target; // only used in FIXEDTARGET mode
	Vector3		m_relative_pos; // also only used for FIXEDTARGET mode

	float		m_move_speed = 0.03f; // only used in FREE mode
	float		m_rotate_speed = 0.0015f; // used in FREE mode and FIXEDTARGET mode
};