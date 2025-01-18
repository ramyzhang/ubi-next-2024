#include "stdafx.h"
#include "SPhysics.h"

void SPhysics::Update(const float deltaTime) {
	// update all the rigidbodies and physics-related components!
	for (EntityID e : EntityView<CTransform, CCollider>(SEntityManager::Instance())) {
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);
		CCollider* ccollider = SEntityManager::Instance().GetComponent<CCollider>(e);
		CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(e);
		
		if (!crb->is_static) ApplyGravity(crb, ctrans, deltaTime * m_multiplier);
		crb->velocity.scale(pow(crb->drag, deltaTime * m_multiplier)); // apply drag
		ctrans->position.add(crb->velocity * deltaTime * m_multiplier); // update position!
		ccollider->center = ctrans->position; // update collider position

		// if it's very slow, just stop it
		if (crb->velocity.magnitude_squared() < 0.01f) crb->velocity = Vector3();
	}

	// perform collision resolution
	SCollision::Instance().BroadUpdate(deltaTime);
	SCollision::Instance().NarrowUpdate(deltaTime);
}

void SPhysics::ApplyGravity(CRigidBody* crb, CTransform* ctrans, const float deltaTime) {
	crb->force.add(m_gravity * crb->mass); // f = m * a babyyy
	crb->velocity.add(crb->force / crb->mass * deltaTime);
	crb->force = Vector3();
}
