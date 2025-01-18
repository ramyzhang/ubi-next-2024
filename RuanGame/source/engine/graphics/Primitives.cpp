#include "stdafx.h"

#include "Primitives.h"

// anonymous namespace to put all helper functions
namespace {
	void FillBottomTriangle(
		const Vector3& side_l,
		const Vector3& top,
		const Vector3& side_r,
		const Vector3& color,
		const Vector3& dither
	) {
		if (abs(side_l.y - top.y) < 0.1f || abs(side_r.y - top.y) < 0.1f) {
			return; // avoid divide by 0
		}
		float dxdy_left = (side_l.x - top.x) / (side_l.y - top.y);
		float dxdy_right = (side_r.x - top.x) / (side_r.y - top.y);

		float curr_x_l = side_l.x - 1.0f;
		float curr_x_r = side_r.x + 1.0f;

		float pixel = 0;

		// start from the bottom and draw horizontal lines up the triangle
		for (int y = static_cast<int>(side_l.y); y < static_cast<int>(top.y); y++)
		{
			if (curr_x_l - 1.0f > curr_x_r) break;

			// this adds a fun dithering effect :D
			if (y % 4 < 2) App::DrawLine(curr_x_l, side_l.y + pixel, curr_x_r, side_l.y + pixel, color.x, color.y, color.z);
			else App::DrawLine(curr_x_l, side_l.y + pixel, curr_x_r, side_l.y + pixel, dither.x, dither.y, dither.z);
			
			curr_x_l += dxdy_left;
			curr_x_r += dxdy_right;
			pixel = pixel + 1.0f;
		}
	}

	void FillTopTriangle(
		const Vector3& bottom,
		const Vector3& side_l,
		const Vector3& side_r,
		const Vector3& color,
		const Vector3& dither
	) {
		if (abs(side_l.y - bottom.y) < 0.1f || abs(side_r.y - bottom.y) < 0.1f) {
			return; // avoid divide by 0
		}
		float dxdy_left = (side_l.x - bottom.x) / (side_l.y - bottom.y);
		float dxdy_right = (side_r.x - bottom.x) / (side_r.y - bottom.y);

		float curr_x_l = side_l.x - 1.0f;
		float curr_x_r = side_r.x + 1.0f;

		float pixel = 0;

		// start from the top and draw horizontal lines down the triangle
		for (int y = static_cast<int>(side_l.y); y >= static_cast<int>(bottom.y); y--)
		{
			if (curr_x_l - 1.0f > curr_x_r) break;

			// this adds a fun dithering effect :D
			if (y % 4 < 2) App::DrawLine(curr_x_l, side_l.y - pixel, curr_x_r, side_l.y - pixel, color.x, color.y, color.z);
			else App::DrawLine(curr_x_l, side_l.y - pixel, curr_x_r, side_l.y - pixel, dither.x, dither.y, dither.z);

			curr_x_l -= dxdy_left;
			curr_x_r -= dxdy_right;
			pixel = pixel + 1.0f;
		}
	}
}

// Using the flat-side triangle algorithm to fill the triangle!
void Triangle::FillTriangle(const Vector3& color) const {
	std::vector<Vector3> v(verts, verts + sizeof verts / sizeof verts[0]);
	Vector3 dither_colour = Vector3(
		std::min(color.x * 1.5f, 1.0f),
		std::min(color.y * 1.5f, 1.0f),
		std::min(color.z * 1.5f, 1.0f));

	std::sort(v.begin(), v.end(), [](Vector3& a, Vector3& b) {
		return a.y < b.y;
		});

	// first, check for trivial case of bottom-flat tri
	if (v[0].y == v[1].y) {
		if (v[0].x < v[1].x) ::FillBottomTriangle(v[0], v[2], v[1], color, dither_colour);
		else ::FillBottomTriangle(v[1], v[2], v[0], color, dither_colour);
	}
	// then, check for trivial case of top-flat tri
	else if (v[1].y == v[2].y) {
		if (v[1].x < v[2].x) ::FillTopTriangle(v[0], v[1], v[2], color, dither_colour);
		else ::FillTopTriangle(v[0], v[2], v[1], color, dither_colour);
	}
	// if not, then split it by two flat tris
	else {
		Vector3 new_point = Vector3(v[2].x + ((v[1].y - v[2].y) / (v[0].y - v[2].y)) * (v[0].x - v[2].x), v[1].y, v[1].z);
		if (v[1].x < new_point.x) {
			::FillBottomTriangle(v[1], v[2], new_point, color, dither_colour);
			::FillTopTriangle(v[0], v[1], new_point, color, dither_colour);
		}
		else {
			::FillBottomTriangle(new_point, v[2], v[1], color, dither_colour);
			::FillTopTriangle(v[0], new_point, v[1], color, dither_colour);
		}
	}

	// DrawTriangleOutline(color);	
}

void Triangle::DrawTriangleOutline(const Vector3& color) const {
	App::DrawLine(
		verts[0].x,
		verts[0].y,
		verts[1].x,
		verts[1].y,
		color.x, color.y, color.z);

	App::DrawLine(
		verts[1].x,
		verts[1].y,
		verts[2].x,
		verts[2].y,
		color.x, color.y, color.z);

	App::DrawLine(
		verts[2].x,
		verts[2].y,
		verts[0].x,
		verts[0].y,
		color.x, color.y, color.z);
}

bool Model::LoadMeshFromFile(const char* fileName) {
	FILE* file;
	file = fopen(fileName, "r");
	
	if (file == nullptr) return false;

	std::vector<Vector3> vecbuf;

	bool ended = false;
	while (!ended) {
		// read a line
		char head[128];

		if (fscanf(file, "%s", head) == EOF) {
			OutputDebugStringA("File has finished!\n");
			ended = true; break; // read the first word
		}

		// it's a comment, skip this line
		if (strcmp(head, "#") == 0) { 
			continue; 
		}
		// read in vectors
		else if (strcmp(head, "v") == 0) { 
			Vector3 newvec = Vector3();

			int res = fscanf(file, "%f %f %f\n", &newvec.x, &newvec.y, &newvec.z);
			if (res == EOF || res != 3) {
				OutputDebugStringA("Couldn't read .obj file vertices!\n");
				ended = true;
				break;
			}

			vecbuf.push_back(newvec);
		}
		// read in faces (triangles)
		else if (strcmp(head, "f") == 0) { 
			Triangle newtri = Triangle();
			int v0, v1, v2;

			int res = fscanf(file, "%d %d %d\n", &v0, &v1, &v2);
			if (res == EOF || res != 3) {
				OutputDebugStringA("Couldn't read .obj file faces!\n");
				ended = true;
				break;
			}

			newtri.verts[0] = vecbuf[v0 - 1];
			newtri.verts[1] = vecbuf[v1 - 1];
			newtri.verts[2] = vecbuf[v2 - 1];
			this->tris.push_back(newtri);
		}
	}

	if (vecbuf.size() == 0) {
		OutputDebugStringA("No vectors found.\n");
		return false;
	}

	return true;
}