#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

#include "App\app.h"
#include "math\Vector3.h"
#include "math\Matrix4x4.h"

struct Triangle {
	Vector3		verts[3];
	// Vector3		normal; // no point in storing it here tbh, it only makes everything perform worse
	float		light_sim; // dot product with the lighting source for colour calcs

	void FillTriangle(const Vector3& color) const;
	void DrawTriangleOutline(const Vector3& color) const;
};

struct Model {
	std::vector<Triangle>	tris;

	bool LoadMeshFromFile(const char* fileName); // has to be an .obj file
};