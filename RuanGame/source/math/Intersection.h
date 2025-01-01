#pragma once

#include "Vector3.h"

inline Vector3 IntersectLinePlane(const Vector3& p0, const Vector3& p1, const Vector3& plane_p, Vector3& plane_n) {
	plane_n.normalize();

	float ndotp = -plane_n.dot(plane_p);
	float ad = p0.dot(plane_n);
	float bd = p1.dot(plane_n);
	float t = (-ndotp - ad) / (bd - ad);

	Vector3 line = p1 - p0;
	Vector3 intersect = line * t;

	return (p0 + intersect);
}