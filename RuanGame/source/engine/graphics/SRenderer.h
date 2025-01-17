#pragma once

#include <algorithm>
#include <deque>
#include "Primitives.h"
#include "core\System.h"
#include "math\Matrix4x4.h"
#include "math\Vector3.h"
#include "math\Intersection.h"
#include "App\main.h"
#include "engine\ecs\ecsutils\EntityView.h"
#include "SCamera.h"

class SRenderer : public System<SRenderer> {
public:
	void Init();
	void Update(const float deltaTime);
	void Render();
	// void Shutdown();

	struct RenderObject {
		EntityID id;
		float z_depth;
	};

protected:
	void DrawMesh(const CMesh& mesh, const CTransform& transform);
	void DrawCollider(const CCollider& collider);
	std::vector<Triangle> ClipTriangle(const Vector3& plane_p, Vector3& plane_n, const Triangle& in); // outputs 0, 1, or 2 triangles

private:
	std::vector<RenderObject> m_render_queue;
};