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
		Vector3		half_size;  // for box - instead of doing min and max, we're going to keep the extents of everything
	};
	// just using union instead of std::variant because it's way faster
};
