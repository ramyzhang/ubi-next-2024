#include "stdafx.h"
#include "SPhysics.h"

void SPhysics::Update(const float deltaTime) {
	// perform collision resolution
	SCollision::Instance().BroadUpdate(deltaTime);
	SCollision::Instance().NarrowUpdate(deltaTime);

	// update all the rigidbodies and physics-related components!
	for (EntityID e : EntityView<CTransform, CCollider>(SEntityManager::Instance())) {
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);
		CCollider* ccollider = SEntityManager::Instance().GetComponent<CCollider>(e);
		
		if (SEntityManager::Instance().GetEntity(e).CheckComponent(CRIGIDBODY)) {
			CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(e);

			// apply gravity if we want
			if (crb->has_gravity) ApplyGravity(crb, ctrans, deltaTime * m_multiplier);
			else crb->velocity.add(crb->force / crb->mass * deltaTime * m_multiplier);

			crb->velocity.scale(pow(crb->drag, deltaTime * m_multiplier)); // apply drag
			ctrans->position.add(crb->velocity * deltaTime * m_multiplier); // update position!

			// if it's very slow, just stop it
			if (crb->velocity.magnitude_squared() < 0.01f) crb->velocity = Vector3();

			crb->force = Vector3(); // reset net force
		}
		
		ccollider->center = ctrans->position; // update collider position
	}
}

void SPhysics::ApplyGravity(CRigidBody* crb, CTransform* ctrans, const float deltaTime) {
	crb->force.add(m_gravity * crb->mass); // f = m * a babyyy
	crb->velocity.add(crb->force / crb->mass * deltaTime);
}
