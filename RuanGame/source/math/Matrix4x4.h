#pragma once

#include "Vector3.h"

struct Matrix4x4 {

	float mat[4][4];
	
	Matrix4x4() : mat{
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0,  0, 0, 1}
	} {}; // to be safe, the initial matrix should always be the identity

	Matrix4x4(float _value) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				mat[i][j] = _value;
			}
		}
	}; // Populate it with a value upon construction

	float* operator [] (const int& index) { return mat[index]; };

	Vector3 operator * (const Vector3& rhs) const; // note: this is for row vector parameters, not column vector
	Matrix4x4 operator * (const Matrix4x4& rhs) const;
	
	Matrix4x4& times(const Matrix4x4& rhs); // this one, we can chain
	Matrix4x4& rotate(const Vector3& euler); // multiplies current matrix by rotation (uses the euler angles alpha, beta, and gamma)
	Matrix4x4& translate(const Vector3& rhs);
	Matrix4x4& pointAt(const Vector3& pos, const Vector3& target, const Vector3& up);
	Matrix4x4& dirtyInvert(); // only for rotation & translation matrices

};

inline Vector3 Matrix4x4::operator * (const Vector3& rhs) const {
	Vector3 res;

	res.x = rhs.x * mat[0][0] + rhs.y * mat[1][0] + rhs.z * mat[2][0] + rhs.w * mat[3][0];
	res.y = rhs.x * mat[0][1] + rhs.y * mat[1][1] + rhs.z * mat[2][1] + rhs.w * mat[3][1];
	res.z = rhs.x * mat[0][2] + rhs.y * mat[1][2] + rhs.z * mat[2][2] + rhs.w * mat[3][2];
	res.w = rhs.x * mat[0][3] + rhs.y * mat[1][3] + rhs.z * mat[2][3] + rhs.w * mat[3][3];

	return res;
}

inline Matrix4x4 Matrix4x4::operator * (const Matrix4x4& rhs) const {
	Matrix4x4 prod;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			prod.mat[i][j] = mat[i][0] * rhs.mat[0][j] + mat[i][1] * rhs.mat[1][j] + mat[i][2] * rhs.mat[2][j] + mat[i][3] * rhs.mat[3][j];
		}
	}

	return prod;
}

inline Matrix4x4& Matrix4x4::times(const Matrix4x4& rhs) {
	Matrix4x4 copy = *this;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = copy.mat[i][0] * rhs.mat[0][j] + copy.mat[i][1] * rhs.mat[1][j] + copy.mat[i][2] * rhs.mat[2][j] + copy.mat[i][3] * rhs.mat[3][j];
		}
	}

	return *this;
}

inline Matrix4x4& Matrix4x4::rotate(const Vector3& euler) {
	Matrix4x4 rotation = Matrix4x4();

	rotation.mat[0][0] = cosf(euler.y) * cosf(euler.z);
	rotation.mat[0][1] = cosf(euler.y) * sinf(euler.z);
	rotation.mat[0][2] = -sinf(euler.y);
		  
	rotation.mat[1][0] = sinf(euler.x) * sinf(euler.y) * cosf(euler.z) - cosf(euler.x) * sinf(euler.z);
	rotation.mat[1][1] = sinf(euler.x) * sinf(euler.y) * sinf(euler.z) + cosf(euler.x) * cosf(euler.z);
	rotation.mat[1][2] = sinf(euler.x) * cosf(euler.y);
		  
	rotation.mat[2][0] = cosf(euler.x) * sinf(euler.y) * cosf(euler.z) + sinf(euler.x) * sinf(euler.z);
	rotation.mat[2][1] = cosf(euler.x) * sinf(euler.y) * sinf(euler.z) - sinf(euler.x) * cosf(euler.z);
	rotation.mat[2][2] = cosf(euler.x) * cosf(euler.y);
		  
	rotation.mat[3][3] = 1.0f;

	rotation = *this * rotation;

	memcpy(mat, rotation.mat, 4 * 4 * sizeof(float));

	return *this;
}

inline Matrix4x4& Matrix4x4::translate(const Vector3& rhs)
{
	Matrix4x4 translation = Matrix4x4();

	translation.mat[0][0] = 1.0f;
	translation.mat[1][1] = 1.0f;
	translation.mat[2][2] = 1.0f;

	translation.mat[3][0] = rhs.x;
	translation.mat[3][1] = rhs.y;
	translation.mat[3][2] = rhs.z;
	translation.mat[3][3] = 1.0f;

	translation = *this * translation;

	memcpy(mat, translation.mat, 4 * 4 * sizeof(float));

	return *this;
}

inline Matrix4x4& Matrix4x4::pointAt(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	// new forward direction
	Vector3 newz = target - pos;
	newz.normalize();

	// new up direction
	Vector3 a = newz * (up.dot(newz));
	Vector3 newy = up - a;
	
	// new right direction
	Vector3 newx = newy.cross(newz);

	Matrix4x4 pointAt = Matrix4x4();

	pointAt.mat[0][0] = newx.x;
	pointAt.mat[0][1] = newx.y;
	pointAt.mat[0][2] = newx.z;

	pointAt.mat[1][0] = newy.x;
	pointAt.mat[1][1] = newy.y;
	pointAt.mat[1][2] = newy.z;

	pointAt.mat[2][0] = newz.x;
	pointAt.mat[2][1] = newz.y;
	pointAt.mat[2][2] = newz.z;

	pointAt.mat[3][0] = pos.x;
	pointAt.mat[3][1] = pos.y;
	pointAt.mat[3][2] = pos.z;

	pointAt = *this * pointAt;

	memcpy(mat, pointAt.mat, 4 * 4 * sizeof(float));

	return *this;
}

inline Matrix4x4& Matrix4x4::dirtyInvert() {
	Matrix4x4 copy = *this;

	mat[0][0] = copy.mat[0][0];
	mat[0][1] = copy.mat[1][0];
	mat[0][2] = copy.mat[2][0];
	mat[0][3] = 0.0f;

	mat[1][0] = copy.mat[0][1];
	mat[1][1] = copy.mat[1][1];
	mat[1][2] = copy.mat[2][1];
	mat[1][3] = 0.0f;
	
	mat[2][0] = copy.mat[0][2];
	mat[2][1] = copy.mat[1][2];
	mat[2][2] = copy.mat[2][2];
	mat[2][3] = 0.0f;
	
	Vector3 trans = Vector3(copy.mat[3][0], copy.mat[3][1], copy.mat[3][2]);
	Vector3 xaxis = Vector3(copy.mat[0][0], copy.mat[0][1], copy.mat[0][2]);
	Vector3 yaxis = Vector3(copy.mat[1][0], copy.mat[1][1], copy.mat[1][2]);
	Vector3 zaxis = Vector3(copy.mat[2][0], copy.mat[2][1], copy.mat[2][2]);

	mat[3][0] = -trans.dot(xaxis);
	mat[3][1] = -trans.dot(yaxis);
	mat[3][2] = -trans.dot(zaxis);
	mat[3][3] = 1.0f;

	return *this;
}
