#include "stdafx.h"
#include "SCollision.h"

// helper functions for getting more collision info!
namespace {
	void SphereCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
		Vector3 diff = o.center - e.center; // get the distance between the centers
		float distance = diff.magnitude();
		normal = diff * (1.0f / distance); // that's the normal!
		depth = (e.radius + o.radius) - distance; // get the depth of penetration
	}

	void AABBCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
		Vector3 overlap = (e.half_size + o.half_size) - (o.center - e.center).abs(); // how much is it overlapping?

		Vector3 diff = o.center - e.center;

		depth = min(min(overlap.x, overlap.y), overlap.z); // get the depth along the shortest axis
		normal = Vector3();
		if (overlap.x < overlap.y&& overlap.x < overlap.z) { // get the axes of the normal
			normal.x = diff.x > 0 ? 1.0f : -1.0f;
		}
		else if (overlap.y < overlap.z) {
			normal.y = diff.y > 0 ? 1.0f : -1.0f;
		}
		else {
			normal.z = diff.z > 0 ? 1.0f : -1.0f;
		}
	}

	void AABBvsSphereCollisionInfo(const CCollider& box, const CCollider& sphere, Vector3& normal, float& depth) {
		Vector3 center = sphere.center;
		Vector3 closest = center.clamp(box.center - box.half_size, box.center + box.half_size); // similar to the spherevssphere
		Vector3 diff = center - closest;
		float distance = diff.magnitude();
		normal = diff * (1.0f / distance);
		depth = sphere.radius - distance;
	}
}

void SCollision::BroadUpdate(const float deltaTime) {
	// just a n^2 loop... sadly
	for (EntityID e : EntityView<CTransform, CCollider, CRigidBody>(SEntityManager::Instance())) {
		CCollider* ecollider = SEntityManager::Instance().GetComponent<CCollider>(e);

		for (EntityID o : EntityView<CTransform, CCollider, CRigidBody>(SEntityManager::Instance())) {
			if (o == e) continue;

			CCollider* ocollider = SEntityManager::Instance().GetComponent<CCollider>(o);

			// queue all of the collided pairs into a vector, and store their info for the narrow phase
			if (ocollider->CheckCollision(*ecollider)) {
				CTransform* etrans = SEntityManager::Instance().GetComponent<CTransform>(e);
				CRigidBody* erb = SEntityManager::Instance().GetComponent<CRigidBody>(e);

				CTransform* otrans = SEntityManager::Instance().GetComponent<CTransform>(o);
				CRigidBody* orb = SEntityManager::Instance().GetComponent<CRigidBody>(o);

				CollisionObject co;
				co.ecollider = ecollider;
				co.ocollider = ocollider;
				co.etrans = etrans;
				co.otrans = otrans;
				co.erb = erb;
				co.orb = orb;

				GetCollisionInfo(*ecollider, *ocollider, co.normal, co.depth);

				m_collided_pairs.push_back(co);

				// TODO: notify your subscribersss
			}
		}
	}
}

// solve the collision! :) just basic impulse
void SCollision::NarrowUpdate(const float deltaTime) {
    for (CollisionObject& co : m_collided_pairs) {
        Vector3 relative_velo = co.orb->velocity - co.erb->velocity;

        // collision impulse magnitude (j)
        float restitution = min(co.erb->restitution, co.orb->restitution);
        float j = -(1.0f + restitution) * relative_velo.dot(co.normal); // reverse and scale the collision depending on bounciness
        j /= (1.0f / co.erb->mass + 1.0f / co.orb->mass); // divide by inverse mass to account for weight

        // apply impulse to velocities
		if (!co.erb->is_static) {
			co.erb->velocity.subtract(co.normal * (1.0f / co.erb->mass) * j);
		}
		if (!co.orb->is_static) {
			co.orb->velocity.add(co.normal * (1.0f / co.orb->mass) * j);
		}
            
        // positional correction (prevents sinking)
        const float percent = 0.8f; // penetration recovery rate
        const float slop = 0.01f;   // penetration allowance
        Vector3 correction = co.normal * max(co.depth - slop, 0.0f) * percent
			* (1.0f / (1.0f / co.erb->mass + 1.0f / co.orb->mass));
		
		// apply position correction depending on mass
		if (!co.erb->is_static) {
			co.etrans->position.subtract(correction * co.erb->mass);
		}
		if (!co.orb->is_static) {
			co.otrans->position.add(correction * co.orb->mass);
		}
    }

    m_collided_pairs.clear();
}

// puts the collision normal and depth magnitude into the parameter references
void SCollision::GetCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
	switch (e.volume_type) {
		case AABB:
			if (o.volume_type == AABB) ::AABBCollisionInfo(e, o, normal, depth);
			else ::AABBvsSphereCollisionInfo(e, o, normal, depth);
			break;
		case SPHERE:
			if (o.volume_type == AABB) ::AABBvsSphereCollisionInfo(o, e, normal, depth);
			else ::SphereCollisionInfo(e, o, normal, depth);
			break;
		default:
			break;
	}
}
