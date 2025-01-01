#pragma once

#include <stdio.h>
#include <vector>
#include <string>

#include "math\Vector3.h"
#include "math\Matrix4x4.h"

struct Triangle {
	Vector3 verts[3];
	Vector3 normal;
	float light_sim; // dot product with the lighting source for colour calcs
};

struct Mesh {
	std::vector<Triangle> tris;
	Matrix4x4 projection;
	Vector3 color;
	
	bool LoadMeshFromFile(const char* fileName, const Vector3& rgb); // has to be an .obj file
};

struct Camera {
	Vector3 position;
	Vector3 look;
	float yaw; // in radians
};