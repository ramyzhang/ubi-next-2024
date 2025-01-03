#include "stdafx.h"

#include "SRenderer.h"

static SRenderer& sRenderer = SRenderer::Instance();

void SRenderer::Init() {
	if (!testMesh.LoadMeshFromFile("data/axis.obj", Vector3(0.92f, 0.25f, 0.2f))) {
		OutputDebugStringA("Couldn't load object from file!\n");
	}

	testMesh.projection[0][0] = m_aspectratio * m_fovrad;
	testMesh.projection[1][1] = m_fovrad;
	testMesh.projection[2][2] = m_far / (m_far - m_near);
	testMesh.projection[2][3] = 1;
	testMesh.projection[3][2] = (-m_far * m_near) / (m_far - m_near);

	depth_buffer = new float[WINDOW_WIDTH, WINDOW_HEIGHT];
}

void SRenderer::Update(const float deltaTime) {
	/*float y = (20 / deltaTime) * (float)M_PI / 180.0f;
	float x = (10 / deltaTime) * (float)M_PI / 180.0f;
	Matrix4x4 transform = Matrix4x4().rotate(Vector3(x, y, 0));*/

	/*for (int i = 0; i < testMesh.tris.size(); i++) {
		for (int j = 0; j < 3; j++) {
			testMesh.tris[i].verts[j] = transform * testMesh.tris[i].verts[j];
		}
	}*/
}

void SRenderer::Render() {
	std::vector<Triangle> trisToDraw;

	for (int i = 0; i < testMesh.tris.size(); i++) {
		Triangle moved_tri;
		Triangle viewed_tri;
		Triangle projected_tri;

		// displace the mesh backwards a bit
		moved_tri.verts[0] = testMesh.tris[i].verts[0] + Vector3(0, 0, 30.0f);
		moved_tri.verts[1] = testMesh.tris[i].verts[1] + Vector3(0, 0, 30.0f);
		moved_tri.verts[2] = testMesh.tris[i].verts[2] + Vector3(0, 0, 30.0f);

		// calculate the normal
		Vector3 normal, line1, line2, camtotri;

		line1 = moved_tri.verts[1] - moved_tri.verts[0];
		line2 = moved_tri.verts[2] - moved_tri.verts[0];
		normal = line1.cross(line2);
		normal.normalize();
		camtotri = moved_tri.verts[0] - camera.position;

		// cull the backfaces
		float dotprod = normal.dot(camtotri);

		if (dotprod >= 0) continue;

		// get new lighting
		Vector3 light_dir = Vector3(0, 1.0f, -1.0f).normalize();
		float light_sim = max(0.1f, light_dir.dot(normal));

		// convert from world space to camera space
		Vector3 up = Vector3(0, 1, 0);
		Vector3 target = Vector3(0, 0, 1);
		camera.look = Matrix4x4().rotate(Vector3(0, camera.yaw, 0)) * target;
		target = camera.position + camera.look;

		Matrix4x4 view_mat = Matrix4x4().pointAt(camera.position, target, up).dirtyInvert();

		viewed_tri.verts[0] = view_mat * moved_tri.verts[0];
		viewed_tri.verts[1] = view_mat * moved_tri.verts[1];
		viewed_tri.verts[2] = view_mat * moved_tri.verts[2];
		viewed_tri.light_sim = light_sim;
		viewed_tri.normal = normal;

		std::vector<Triangle> clipped_tris = ClipTriangle(Vector3(0, 0, 0.1f), Vector3(0, 0, 1.0f), viewed_tri);

		// perform the projection from 3d to 2d
		for (Triangle& tri : clipped_tris) {
			projected_tri.verts[0] = testMesh.projection * tri.verts[0];
			projected_tri.verts[0].divideByW();
			projected_tri.verts[1] = testMesh.projection * tri.verts[1];
			projected_tri.verts[1].divideByW();
			projected_tri.verts[2] = testMesh.projection * tri.verts[2];
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

			trisToDraw.push_back(projected_tri);
		}
	}
	
	// TODO: proper z-buffer
	std::sort(trisToDraw.begin(), trisToDraw.end(), [](Triangle& a, Triangle& b)
		{
			float za = (a.verts[0].z + a.verts[1].z + a.verts[2].z) / 3.0f;
			float zb = (b.verts[0].z + b.verts[1].z + b.verts[2].z) / 3.0f;
			return za > zb;
		});

	for (auto &tri : trisToDraw) {
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
			Vector3 litcolor = testMesh.color * abs(draw_tri.light_sim);
			// Vector3 color = Vector3(abs(tri.normal.x), abs(tri.normal.y), abs(tri.normal.z));

			if (wireframe_view) {
				App::DrawLine(
					draw_tri.verts[0].x,
					draw_tri.verts[0].y,
					draw_tri.verts[1].x,
					draw_tri.verts[1].y,
					1, 1, 1);

				App::DrawLine(
					draw_tri.verts[1].x,
					draw_tri.verts[1].y,
					draw_tri.verts[2].x,
					draw_tri.verts[2].y,
					1, 1, 1);

				App::DrawLine(
					draw_tri.verts[2].x,
					draw_tri.verts[2].y,
					draw_tri.verts[0].x,
					draw_tri.verts[0].y,
					1, 1, 1);
			}
			else {
				DrawTriangle(draw_tri, litcolor);
			}
		}
	}
}

void SRenderer::DrawTriangle(const Triangle& tri, const Vector3& rgb) {
	float v0x = tri.verts[0].x;		float v0y = tri.verts[0].y;
	float v1x = tri.verts[1].x;		float v1y = tri.verts[1].y;
	float v2x = tri.verts[2].x;		float v2y = tri.verts[2].y;

#if APP_USE_VIRTUAL_RES		
	APP_VIRTUAL_TO_NATIVE_COORDS(v0x, v0y);
	APP_VIRTUAL_TO_NATIVE_COORDS(v1x, v1y);
	APP_VIRTUAL_TO_NATIVE_COORDS(v2x, v2y);
#endif

	glBegin(GL_TRIANGLES);
	glColor3f(rgb.x, rgb.y, rgb.z);
	glVertex2f(v0x, v0y);
	glVertex2f(v1x, v1y);
	glVertex2f(v2x, v2y);
	glEnd();
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