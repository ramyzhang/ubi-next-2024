#include "stdafx.h"

#include "Primitives.h"
#include <windows.h> // TODO: remove this

bool Mesh::LoadMeshFromFile(const char* fileName, const Vector3& rgb) {
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

		if (strcmp(head, "#") == 0) { // it's a comment, skip this line
			continue; 
		} else if (strcmp(head, "v") == 0) { // read in vectors
			Vector3 newvec = Vector3();

			int res = fscanf(file, "%f %f %f\n", &newvec.x, &newvec.y, &newvec.z);
			if (res == EOF || res != 3) {
				OutputDebugStringA("Couldn't read .obj file vertices!\n");
				ended = true;
				break;
			}

			vecbuf.push_back(newvec);
		} else if (strcmp(head, "f") == 0) { // read in faces (triangles)
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

	this->color = rgb;

	return true;
}