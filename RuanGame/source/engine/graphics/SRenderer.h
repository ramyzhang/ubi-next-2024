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

	// just a debug utility for visualizing lines in worldspace
	void DrawDebugLine(Vector3 start, Vector3 end, Vector3 colour = Vector3(1.0f, 1.0f, 1.0f)) const;
	
	// hard coded the entityid for the level floor because I don't want to implement quadtree painter's algo hehe ;A;
	void SetFloorEntity(EntityID& floor_e) { m_floor = floor_e; };

protected:
	void					DrawMesh(const CMesh& mesh, const CTransform& transform);
	void					DrawDebugCollider(const CCollider& collider) const; // yet another debug utility

	// ClipTriangle outputs 0, 1, or 2 triangles depending on the clipping results
	std::vector<Triangle>	ClipTriangle(const Vector3& plane_p, Vector3& plane_n, const Triangle& in); 

private:
	std::vector<RenderObject>	m_render_queue;
	Vector3						m_offset = Vector3(1.0f, 1.0f, 0.0f);
	Vector3						m_light_dir = Vector3(0, 1.0f, -1.0f);

	EntityID					m_floor = MAX_ENTITIES;
};