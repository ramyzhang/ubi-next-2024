#pragma once

#include <algorithm>
#include <deque>
#include "Primitives.h"
#include "core\System.h"
#include "math\Matrix4x4.h"
#include "math\Vector3.h"
#include "math\Intersection.h"
#include "App\main.h"
#include "App\app.h"

class SRenderer : public System<SRenderer> {

public:
	void Init();
	void Update(const float deltaTime);
	void Render();
	// void Shutdown();

	Camera camera;
	bool wireframe_view;

protected:
	void DrawTriangle(const Triangle& tri, const Vector3& rgb);
	std::vector<Triangle> ClipTriangle(const Vector3& plane_p, Vector3& plane_n, const Triangle& in); // outputs 0, 1, or 2 triangles

private:
	Model	testMesh; // TODO: remove this

	float*	m_depth_buffer = nullptr;

	float	m_near = 0.1f;
	float	m_far = 1000;
	float	m_fov = 90;
	float	m_aspectratio = (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH;
	float	m_fovrad = 1 / tanf(m_fov * 0.5f / 180.0f * (float)M_PI);
};