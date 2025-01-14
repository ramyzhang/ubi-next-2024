#include "stdafx.h"

#include "SRenderer.h"

void SRenderer::Init() {
	SCamera::Instance().Init();
}

void SRenderer::Update(const float deltaTime) {
	m_render_queue.clear();
	SCamera::Instance().HandleInputs(deltaTime);
	
	// set up render buffer such that farthest objects are drawn before closest objects
	for (EntityID e : EntityView<CTransform, CMesh>(SEntityManager::Instance())) {
		CMesh* cmesh = SEntityManager::Instance().GetComponent<CMesh>(e);
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);

		Vector3 camera_space = SCamera::Instance().view * ctrans->position;

		m_render_queue.push_back({
			e,
			camera_space.z  // Now using camera-space z value
			});
	}

	std::sort(m_render_queue.begin(), m_render_queue.end(), [](RenderObject& a, RenderObject& b) {
		return a.z_depth > b.z_depth;
		});
}

void SRenderer::Render() {
	// iterate through all render objects and draw them!
	for (const auto& obj : m_render_queue) {
		CMesh* cmesh = SEntityManager::Instance().GetComponent<CMesh>(obj.id);
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(obj.id);
		DrawMesh(*cmesh, *ctrans);
	}
}

void SRenderer::DrawMesh(const CMesh& mesh, const CTransform& transform) {
	std::vector<Triangle> tris_to_draw;

	for (int i = 0; i < mesh.model->tris.size(); i++) {
		Triangle moved_tri;
		Triangle viewed_tri;
		Triangle projected_tri;

		// displace the mesh backwards a bit
		Matrix4x4 world = transform.GetWorldMatrix();
		moved_tri.verts[0] = world * mesh.model->tris[i].verts[0] + Vector3(0, 0, 30.0f);
		moved_tri.verts[1] = world * mesh.model->tris[i].verts[1] + Vector3(0, 0, 30.0f);
		moved_tri.verts[2] = world * mesh.model->tris[i].verts[2] + Vector3(0, 0, 30.0f);

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
		Vector3 light_dir = Vector3(-1.0f, 1.0f, -1.0f).normalize();
		float light_sim = max(0.1f, light_dir.dot(normal));

		// convert from world space to camera space
		viewed_tri.verts[0] = SCamera::Instance().view * moved_tri.verts[0];
		viewed_tri.verts[1] = SCamera::Instance().view * moved_tri.verts[1];
		viewed_tri.verts[2] = SCamera::Instance().view * moved_tri.verts[2];
		viewed_tri.light_sim = light_sim;
		viewed_tri.normal = normal;

		std::vector<Triangle> clipped_tris = ClipTriangle(Vector3(0, 0, 0.1f), Vector3(0, 0, 1.0f), viewed_tri);

		// perform the projection from 3d to 2d
		for (Triangle& tri : clipped_tris) {
			projected_tri.verts[0] = SCamera::Instance().projection * tri.verts[0];
			projected_tri.verts[0].divideByW();
			projected_tri.verts[1] = SCamera::Instance().projection * tri.verts[1];
			projected_tri.verts[1].divideByW();
			projected_tri.verts[2] = SCamera::Instance().projection * tri.verts[2];
			projected_tri.verts[2].divideByW();

			projected_tri.light_sim = tri.light_sim;
			projected_tri.normal = tri.normal;

			Vector3 offset = Vector3(1.0f, 1.0f, 0.0f);

			// offset into screenspace
			for (int j = 0; j < 3; j++) {
				projected_tri.verts[j].add(offset);

				projected_tri.verts[j].x *= 0.5f * (float)WINDOW_WIDTH;
				projected_tri.verts[j].y *= 0.5f * (float)WINDOW_HEIGHT;
			}

			tris_to_draw.push_back(projected_tri);
		}
	}

	// TODO: proper z-buffer?
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
			Vector3 litcolor = mesh.colour * abs(draw_tri.light_sim);

			draw_tri.FillTriangle(litcolor);
		}
	}
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
		newtri.normal = in.normal;
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
		newtri1.normal = in.normal;		newtri1.light_sim = in.light_sim;
		newtri2.normal = in.normal;		newtri2.light_sim = in.light_sim;

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