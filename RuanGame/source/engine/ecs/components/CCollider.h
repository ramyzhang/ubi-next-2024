#pragma once

#include "math/Vector3.h"

struct CCollider {
    Vector3 center; // offset from transform
};

struct CSphereCollider : public CCollider {
    float radius;
};

struct CBoxCollider : public CCollider {
    Vector3 half_size;
};