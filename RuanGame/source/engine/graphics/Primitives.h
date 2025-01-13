#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h> // TODO: remove this

#include "App\app.h"
#include "math\Vector3.h"
#include "math\Matrix4x4.h"

struct Triangle {
	Vector3		verts[3];
	Vector3		normal;
	float		light_sim; // dot product with the lighting source for colour calcs

	void FillTriangle(const Vector3& color) const;
};

struct Model {
	std::vector<Triangle>	tris;

	bool LoadMeshFromFile(const char* fileName); // has to be an .obj file
};