#include "stdafx.h"

#include "SRenderer.h"

void SRenderer::Init() {
	SCamera::Instance().Init();
}

void SRenderer::Update(const float deltaTime) {
	SCamera::Instance().HandleInputs(deltaTime);
	
	// set up render buffer such that farthest objects are drawn before closest objects
	m_render_queue.clear();

	for (EntityID e : EntityView<CTransform, CMesh>(SEntityManager::Instance())) {
		if (e == m_floor) continue;

		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);

		Vector3 camera_space = SCamera::Instance().view * ctrans->position; // this is not very good

		m_render_queue.push_back({
			e,
			camera_space.z  // Now using camera-space z value
			});
	}

	std::sort(m_render_queue.begin(), m_render_queue.end(), [](RenderObject& a, RenderObject& b) {
		return a.z_depth < b.z_depth; // sort in reverse order so i can just append the floor at the end
		});

	// hard coding the floor because I don't want to implement quadtree painter's algo...
	// i'm so sorry...
	if (m_floor >= MAX_ENTITIES) return;

	CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(m_floor);
	Vector3 camera_space = SCamera::Instance().view * ctrans->position;
	m_render_queue.push_back({
		m_floor,
		camera_space.z  // Now using camera-space z value
		});
}

void SRenderer::Render() {
	if (SEntityManager::Instance().GetNumEntities() < 1) FreeContainer(m_render_queue);

	// iterate through all render objects and draw them!
	for (int i = (int)m_render_queue.size() - 1; i >= 0; i--) {
		// since this is a delayed render queue, let's make sure everything still exists
		if (!SEntityManager::Instance().GetEntity(m_render_queue[i].id).IsActive()) return;
		if (!SEntityManager::Instance().GetEntity(m_render_queue[i].id).CheckComponent(CMESH)) return;
		if (!SEntityManager::Instance().GetEntity(m_render_queue[i].id).CheckComponent(CTRANSFORM)) return;

		CMesh* cmesh = SEntityManager::Instance().GetComponent<CMesh>(m_render_queue[i].id);
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(m_render_queue[i].id);

		DrawMesh(*cmesh, *ctrans);
	}

#ifdef _DEBUG 
	// let's draw the colliders in debug mode
	for (EntityID e : EntityView<CCollider, CRigidBody>(SEntityManager::Instance())) {
		CCollider* ccollider = SEntityManager::Instance().GetComponent<CCollider>(e);
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);
		CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(e);

		DrawDebugCollider(*ccollider);
		// DrawDebugLine(ctrans->position, ctrans->position + crb->velocity);
	}

	/*for (EntityID e : EntityView<CBoid>(SEntityManager::Instance())) {
		CBoid* cboid = SEntityManager::Instance().GetComponent<CBoid>(e);
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);

		DrawDebugLine(ctrans->position, ctrans->position + cboid->forward);
	}*/
#else
	// draw the bounds - sadly hardcoded, since I don't want to
	// store the bool in the mesh component
	for (EntityID e : EntityView<CCollider, CRigidBody>(SEntityManager::Instance())) {
		if (SEntityManager::Instance().GetEntity(e).GetTag() != "bound") continue;

		CCollider* ccollider = SEntityManager::Instance().GetComponent<CCollider>(e);
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);
		CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(e);

		DrawDebugCollider(*ccollider);
	}
#endif // _DEBUG
}

void SRenderer::DrawDebugLine(Vector3 start, Vector3 end, Vector3 colour) const {
	Matrix4x4 view = SCamera::Instance().view;
	Matrix4x4 proj = SCamera::Instance().projection;

	Vector3 proj_s = view * proj * start;
	Vector3 proj_e = view * proj * end;

	if (proj_s.z <= 0 || proj_e.z <= 0) return; // early return if it's behind the screen

	proj_s.divide_by_w();
	proj_e.divide_by_w();
	
	proj_s.add(m_offset);
	proj_e.add(m_offset);

	proj_s.x *= 0.5f * (float)WINDOW_WIDTH;
	proj_s.y *= 0.5f * (float)WINDOW_HEIGHT;

	proj_e.x *= 0.5f * (float)WINDOW_WIDTH;
	proj_e.y *= 0.5f * (float)WINDOW_HEIGHT;

	App::DrawLine(proj_s.x, proj_s.y, proj_e.x, proj_e.y, colour.x, colour.y, colour.z);
}

void SRenderer::DrawMesh(const CMesh& mesh, const CTransform& transform) {
	std::vector<Triangle> tris_to_draw;

	for (int i = 0; i < mesh.model->tris.size(); i++) {
		Triangle moved_tri;
		Triangle viewed_tri;
		Triangle projected_tri;

		// translate model vertices to world pos
		Matrix4x4 world = transform.GetWorldMatrix();
		moved_tri.verts[0] = world * mesh.model->tris[i].verts[0];
		moved_tri.verts[1] = world * mesh.model->tris[i].verts[1];
		moved_tri.verts[2] = world * mesh.model->tris[i].verts[2];

		// calculate the normal
		Vector3 normal, line1, line2, camtotri;

		line1 = moved_tri.verts[1] - moved_tri.verts[0];
		line2 = moved_tri.verts[2] - moved_tri.verts[0];
		normal = line1.cross(line2);
		normal.normalize();
		camtotri = moved_tri.verts[0] - SCamera::Instance().position;

		// cull the backfaces
		float dotprod = normal.dot(camtotri);

		if (dotprod >= 0) continue;

		// get new lighting
		float light_sim = std::max(0.35f, m_light_dir.dot(normal));

		// convert from world space to camera space
		viewed_tri.verts[0] = SCamera::Instance().view * moved_tri.verts[0];
		viewed_tri.verts[1] = SCamera::Instance().view * moved_tri.verts[1];
		viewed_tri.verts[2] = SCamera::Instance().view * moved_tri.verts[2];
		viewed_tri.light_sim = light_sim;

		std::vector<Triangle> clipped_tris = ClipTriangle(Vector3(0, 0, 0.1f), Vector3(0, 0, 1.0f), viewed_tri);

		// perform the projection from 3d to 2d
		for (Triangle& tri : clipped_tris) {
			projected_tri.verts[0] = SCamera::Instance().projection * tri.verts[0];
			projected_tri.verts[0].divide_by_w();
			projected_tri.verts[1] = SCamera::Instance().projection * tri.verts[1];
			projected_tri.verts[1].divide_by_w();
			projected_tri.verts[2] = SCamera::Instance().projection * tri.verts[2];
			projected_tri.verts[2].divide_by_w();

			projected_tri.light_sim = tri.light_sim;

			// offset into screenspace
			for (int j = 0; j < 3; j++) {
				projected_tri.verts[j].add(m_offset);

				projected_tri.verts[j].x *= 0.5f * (float)WINDOW_WIDTH;
				projected_tri.verts[j].y *= 0.5f * (float)WINDOW_HEIGHT;
			}

			tris_to_draw.push_back(projected_tri);
		}
	}

	// TODO: proper z-buffer?\
	// sortign the triangles within the mesh so that we draw the mesh itself back to front
	std::sort(tris_to_draw.begin(), tris_to_draw.end(), [](Triangle& a, Triangle& b)
		{
			float za = (a.verts[0].z + a.verts[1].z + a.verts[2].z) / 3.0f;
			float zb = (b.verts[0].z + b.verts[1].z + b.verts[2].z) / 3.0f;
			return za < zb;
		});

	for (auto& tri : tris_to_draw) {
		// clip the triangles against the screen edges
		std::vector<Triangle> clipped;
		std::deque<Triangle> tri_queue;

		tri_queue.push_back(tri);

		size_t new_tris = 1;

		// for the four planes...
		for (int i = 0; i < 4; i++) {
			while (new_tris > 0) {
				// take the triangle from the front of the queue
				Triangle test = tri_queue.front();
				tri_queue.pop_front();
				new_tris--;

				// clip
				switch (i) {
					case 0:
						clipped = ClipTriangle(Vector3(0, 0, 0), Vector3(0, 1.0f, 0), test); break; // bottom
					case 1:
						clipped = ClipTriangle(Vector3(0, (float)WINDOW_HEIGHT - 1.0f, 0), Vector3(0, -1.0f, 0), test); break; // top
					case 2:
						clipped = ClipTriangle(Vector3(0, 0, 0), Vector3(1.0f, 0, 0), test); break; // left
					case 3:
						clipped = ClipTriangle(Vector3((float)WINDOW_WIDTH - 1.0f, 0, 0), Vector3(-1.0f, 0, 0), test); break; // right
				}

				for (int j = 0; j < clipped.size(); j++) {
					tri_queue.push_back(clipped[j]);
				}
			}

			new_tris = tri_queue.size();
		}

		for (Triangle& draw_tri : tri_queue) {
			// apply lighting and draw
			Vector3 litcolour = mesh.colour * abs(draw_tri.light_sim);
			draw_tri.FillTriangle(litcolour);
		}
	}
}

// not perfect, just for visualizing the boxes to debug
void SRenderer::DrawDebugCollider(const CCollider& collider) const {
	Vector3 colour = Vector3(0.502f, 0.098f, 0.098f); // dark red

	// if it's a sphere, just draw 6 lines in each direction
	if (collider.volume_type == SPHERE) {
		Vector3 north = collider.center + Vector3(0, collider.radius, 0);
		Vector3 south = collider.center + Vector3(0, -collider.radius, 0);
		Vector3 east =	collider.center + Vector3(collider.radius, 0, 0);
		Vector3 west =	collider.center + Vector3(-collider.radius, 0, 0);
		Vector3 front = collider.center + Vector3(0, 0, -collider.radius);
		Vector3 back =	collider.center + Vector3(0, 0, collider.radius);

		DrawDebugLine(collider.center, north, colour);
		DrawDebugLine(collider.center, south, colour);
		DrawDebugLine(collider.center, east, colour);
		DrawDebugLine(collider.center, west, colour);
		DrawDebugLine(collider.center, front, colour);
		DrawDebugLine(collider.center, back, colour);

		return;
	}

	// if it's AABB, draw the cube
	Matrix4x4 view = SCamera::Instance().view;
	Matrix4x4 proj = SCamera::Instance().projection;

	// calculate the 8 corners of the box in world space
	Vector3 min = collider.center - collider.half_size;
	Vector3 max = collider.center + collider.half_size;

	Vector3 corners[8] = {
		Vector3(min.x, min.y, min.z, 1.0f), // front bottom left
		Vector3(max.x, min.y, min.z, 1.0f), // front bottom right
		Vector3(max.x, max.y, min.z, 1.0f), // front top right
		Vector3(min.x, max.y, min.z, 1.0f), // front top left
		Vector3(min.x, min.y, max.z, 1.0f), // back bottom left
		Vector3(max.x, min.y, max.z, 1.0f), // back bottom right
		Vector3(max.x, max.y, max.z, 1.0f), // back top right
		Vector3(min.x, max.y, max.z, 1.0f)  // back top left
	};
	
	// project all corners to screen space
	Vector3 projected[8];
	Vector3 offset = Vector3(1.0f, 1.0f, 0.0f);
	for (int i = 0; i < 8; i++) {
		projected[i] = view * proj * corners[i];
		if (projected[i].z <= 0) return; // early return if it's behind the screen

		projected[i].divide_by_w();

		projected[i].add(offset);

		projected[i].x *= 0.5f * (float)WINDOW_WIDTH;
		projected[i].y *= 0.5f * (float)WINDOW_HEIGHT;
	}
	
	// front face
	App::DrawLine(projected[0].x, projected[0].y, projected[1].x, projected[1].y, colour.x, colour.y, colour.z);
	App::DrawLine(projected[1].x, projected[1].y, projected[2].x, projected[2].y, colour.x, colour.y, colour.z);
	App::DrawLine(projected[2].x, projected[2].y, projected[3].x, projected[3].y, colour.x, colour.y, colour.z);
	App::DrawLine(projected[3].x, projected[3].y, projected[0].x, projected[0].y, colour.x, colour.y, colour.z);

	// face
	App::DrawLine(projected[4].x, projected[4].y, projected[5].x, projected[5].y, colour.x, colour.y, colour.z);
	App::DrawLine(projected[5].x, projected[5].y, projected[6].x, projected[6].y, colour.x, colour.y, colour.z);
	App::DrawLine(projected[6].x, projected[6].y, projected[7].x, projected[7].y, colour.x, colour.y, colour.z);
	App::DrawLine(projected[7].x, projected[7].y, projected[4].x, projected[4].y, colour.x, colour.y, colour.z);

	// connecting lines
	App::DrawLine(projected[0].x, projected[0].y, projected[4].x, projected[4].y, colour.x, colour.y, colour.z);
	App::DrawLine(projected[1].x, projected[1].y, projected[5].x, projected[5].y, colour.x, colour.y, colour.z);
	App::DrawLine(projected[2].x, projected[2].y, projected[6].x, projected[6].y, colour.x, colour.y, colour.z);
	App::DrawLine(projected[3].x, projected[3].y, projected[7].x, projected[7].y, colour.x, colour.y, colour.z);
}

std::vector<Triangle> SRenderer::ClipTriangle(const Vector3& plane_p, Vector3& plane_n, const Triangle& in) {
	std::vector<Triangle> res;

	plane_n.normalize();
	
	// check if we are inside or outside the plane for each point
	std::vector<Vector3> inside_points;
	std::vector<Vector3> outside_points;
	
	// small lambda function for plane SDF
	auto signed_dist = [&](Vector3 p) {
			return (plane_n.dot(p) - plane_n.dot(plane_p));
		};

	float d0 = signed_dist(in.verts[0]);
	float d1 = signed_dist(in.verts[1]);
	float d2 = signed_dist(in.verts[2]);
	
	// use SDF to determine
	if (d0 >= 0) { inside_points.push_back(in.verts[0]); }
	else { outside_points.push_back(in.verts[0]); }
	if (d1 >= 0) { inside_points.push_back(in.verts[1]); }
	else { outside_points.push_back(in.verts[1]); }
	if (d2 >= 0) { inside_points.push_back(in.verts[2]); }
	else { outside_points.push_back(in.verts[2]); }
	
	// case 1: the whole triangle is inside visible world
	if (inside_points.size() == 3) { 
		res.push_back(in);
	}
	 
	// case 2: there is one point inside the visible world, and two points outside
	// then, create a new, smaller triangle
	if (inside_points.size() == 1 && outside_points.size() == 2) { //
		Triangle newtri;
		newtri.light_sim = in.light_sim;

		newtri.verts[0] = inside_points[0];
		newtri.verts[1] = IntersectLinePlane(inside_points[0], outside_points[0], plane_p, plane_n);
		newtri.verts[2] = IntersectLinePlane(inside_points[0], outside_points[1], plane_p, plane_n);

		res.push_back(newtri);
	}

	// case 3: there are two points inside the visible world, and one point outside
	// then, create a quad (two triangles)
	if (inside_points.size() == 2 && outside_points.size() == 1) {
		Triangle newtri1;
		Triangle newtri2;
		newtri1.light_sim = in.light_sim;
		newtri2.light_sim = in.light_sim;

		newtri1.verts[0] = inside_points[0];
		newtri1.verts[1] = inside_points[1];
		newtri1.verts[2] = IntersectLinePlane(inside_points[0], outside_points[0], plane_p, plane_n);

		newtri2.verts[0] = inside_points[1];
		newtri2.verts[1] = newtri1.verts[2];
		newtri2.verts[2] = IntersectLinePlane(inside_points[1], outside_points[0], plane_p, plane_n);

		res.push_back(newtri1);
		res.push_back(newtri2);
	}

	return res;
}
