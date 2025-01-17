#pragma once

#include "core/System.h"
#include "engine/ecs/ecsutils/EntityView.h"
#include "SCollision.h"

// physics system has to update the CCollider components to match the new position of the transform
// it's also responsible for adding any velocity (or subtracting it) to update the transform position
class SPhysics : public System<SPhysics> {
public:
	void Update(const float deltaTime);
};