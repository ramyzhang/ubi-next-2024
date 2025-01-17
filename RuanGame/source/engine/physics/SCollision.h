#pragma once

#include "core/System.h"
#include "engine/ecs/ecsutils/EntityView.h"
#include "core/Subject.h"

class SCollision : public System<SCollision>, public Subject {
public:
	void BroadUpdate(const float deltaTime); // really basic broad phase (just n^2 loop)
	void NarrowUpdate(const float deltaTime); // narrow phase here (collision resolution)

	void GetCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth);

	struct CollisionObject {
		CTransform*		etrans;
		CCollider*		ecollider;
		CRigidBody*		erb;

		CTransform*		otrans;
		CCollider*		ocollider;
		CRigidBody*		orb;

		Vector3			normal;
		float			depth;
	};

private:
	std::vector<CollisionObject> m_collided_pairs;
};