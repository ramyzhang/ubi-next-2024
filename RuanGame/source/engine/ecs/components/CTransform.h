#pragma once

#include "math/Vector3.h"

struct CTransform {
	Vector3 position;
	Vector3 rotation;
	Vector3 scale;
	Vector3 velocity;

    Matrix4x4 GetWorldMatrix() const {
        return Matrix4x4().scale(scale).rotate(rotation).translate(position);
    }
};