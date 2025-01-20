#pragma once

#include "math/Vector3.h"

struct CRigidBody {
    Vector3     velocity = Vector3();
    Vector3     force = Vector3();
    float       mass = 1.0f;
    float       bounciness = 0.5f; // 1.0f for max, 0 for min
    float       drag = 0.95f; // it should decelerate eventually
    bool        is_static = false;
    bool        has_gravity = false;
};