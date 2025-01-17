#include "stdafx.h"
#include "SPhysics.h"

void SPhysics::Update(const float deltaTime) {
	// update all the rigidbodies and physics-related components!
	for (EntityID e : EntityView<CTransform, CCollider>(SEntityManager::Instance())) {
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);
		CCollider* ccollider = SEntityManager::Instance().GetComponent<CCollider>(e);
		CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(e);
		
		ctrans->position.add(crb->velocity); // update position!
		ccollider->center = ctrans->position; // update collider position

		crb->velocity.scale(pow(crb->drag, 0.1f * deltaTime)); // apply drag
		if (crb->velocity.magnitude_squared() < 0.01f) { // if it's very slow, just stop it
			crb->velocity = Vector3(0, 0, 0);
		}

		// TODO: gravity??? idk lol
	}

	// perform collision resolution
	SCollision::Instance().BroadUpdate(deltaTime);
	SCollision::Instance().NarrowUpdate(deltaTime);
}
