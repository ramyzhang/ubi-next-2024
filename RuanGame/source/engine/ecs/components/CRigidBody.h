#pragma once

#include "math/Vector3.h"

struct CRigidBody {
    Vector3     velocity;
    float       mass = 1.0f;
    float       restitution = 0.5f; // bounciness
    float       drag = 0.95f; // it should decelerate eventually
    bool        is_static = false;
};