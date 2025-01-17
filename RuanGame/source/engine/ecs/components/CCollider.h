#pragma once

#include "math/Vector3.h"

enum ColliderVolume {
    AABB,
    SPHERE
};

struct CCollider {
	CCollider() : half_size() {}; // need this bc of union with non-trivial member

	ColliderVolume	volume_type;
    Vector3			center; // offset from transform
	union {
		float		radius;      // for sphere
		Vector3		half_size;  // for box
	}; // just using union instead of std::variant because it's way faster

	bool CheckCollision(const CCollider& other) const;
};

// some helper functions for collision
namespace {
	inline bool AABBvsAABBCollision(const CCollider& a, const CCollider& b) {
		// typical aabb collision
		Vector3 minA = a.center - a.half_size;
		Vector3 maxA = a.center + a.half_size;
		Vector3 minB = b.center - b.half_size;
		Vector3 maxB = b.center + b.half_size;

		return (minA.x <= maxB.x && maxA.x >= minB.x) &&
			(minA.y <= maxB.y && maxA.y >= minB.y) &&
			(minA.z <= maxB.z && maxA.z >= minB.z);
	}

	inline bool SpherevsSphereCollision(const CCollider& a, const CCollider& b) {
		// ezpz sphere collision
		float distance = b.center.distance(a.center);
		float minDist = a.radius + b.radius;
		return distance < minDist;
	}

	inline bool AABBvsSphereCollision(const CCollider& box, const CCollider& sphere) {
		// find closest point on aabb to sphere center
		Vector3 center = sphere.center;
		Vector3 closest = center.clamp(box.center - box.half_size, box.center + box.half_size);

		return sphere.center.distance_squared(closest) < (sphere.radius * sphere.radius);
	}
}

inline bool CCollider::CheckCollision(const CCollider& other) const {
	bool res = false;

	switch (volume_type) {
		case AABB:
			if (other.volume_type == AABB) res = ::AABBvsAABBCollision(*this, other);
			else if (other.volume_type == SPHERE) res = ::AABBvsSphereCollision(*this, other);
			break;
		case SPHERE:
			if (other.volume_type == AABB) res = ::AABBvsSphereCollision(other, *this);
			else if (other.volume_type == SPHERE) res = ::SpherevsSphereCollision(*this, other);
			break;
		default:
			break;
	}

	return res;
}




