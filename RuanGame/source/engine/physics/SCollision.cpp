#include "stdafx.h"

#include "SCollision.h"

// helper functions for getting more collision info! (normal and depth)
namespace {
	inline void SphereCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
		Vector3 diff = o.center - e.center; // get the distance between the centers
		float distance = diff.magnitude();
		normal = diff * (1.0f / distance); // that's the normal!
		depth = (e.radius + o.radius) - distance; // get the depth of penetration
	}

	inline void AABBCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
		Vector3 overlap = (e.half_size + o.half_size) - (o.center - e.center).abs(); // how much is it overlapping?

		Vector3 diff = o.center - e.center;

		depth = std::min(std::min(overlap.x, overlap.y), overlap.z); // get the depth along the shortest axis
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

	inline void AABBvsSphereCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
		const CCollider& sphere = e.volume_type == SPHERE ? e : o;
		const CCollider& box = e.volume_type == AABB ? e : o;

		Vector3 center = sphere.center;
		Vector3 closest = center.clamp(box.center - box.half_size, box.center + box.half_size);
		Vector3 diff = closest - sphere.center;

		// check if sphere center is inside AABB, because sometimes it misses the frame before collision
		if (diff.x == 0 && diff.y == 0 && diff.z == 0) {
			// this will tell the collision resolver to make the object flip its velocity vector
			normal = Vector3();
			depth = sphere.radius * 2.0f;
		}
		else {
			float distance = diff.magnitude();
			normal = diff.normalize(); 
			if (!(e.volume_type == SPHERE)) normal = Vector3() - normal; // normal needs to point from e to o

			depth = sphere.radius - distance;
		}
	}
}

void SCollision::BroadUpdate(const float deltaTime) {
	// just a n^2 loop... sadly
	for (EntityID e : EntityView<CTransform, CCollider, CRigidBody>(SEntityManager::Instance())) {
		Entity e_e = SEntityManager::Instance().GetEntity(e);

		CCollider* ecollider = SEntityManager::Instance().GetComponent<CCollider>(e);

		for (EntityID o : EntityView<CTransform, CCollider, CRigidBody>(SEntityManager::Instance())) {
			if (o == e) continue;

			Entity e_o = SEntityManager::Instance().GetEntity(o);

			if (e_e.CheckComponent(CBOID) || e_o.CheckComponent(CBOID)) continue; // I'll deal with the boids in SBoid
			// this is so not clean but sacrificing for performance lol

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

        // calculate the collision impulse magnitude (j)
		Vector3 velo = co.erb->velocity;
		Vector3 normal = co.normal.isZero() ? velo.normalize() : co.normal;

        float bounce = std::min(co.erb->bounciness, co.orb->bounciness);
        float j = -(1.0f + bounce) * relative_velo.dot(normal); // reverse and scale the collision impact depending on bounciness
        j /= (1.0f / co.erb->mass + 1.0f / co.orb->mass); // divide by inverse mass to account for weight
		// if it's heavier, it should be less impacted

        // apply impulse to velocities
		if (!co.erb->is_static) {
			co.erb->velocity.subtract(normal * (1.0f / co.erb->mass) * j);
		}
		if (!co.orb->is_static) {
			co.orb->velocity.add(normal * (1.0f / co.orb->mass) * j);
		}
            
        // prevent it from sinking into the other object
        const float percent = 0.8f; // penetration recovery rate
        const float pene = 0.1f;   // let it penetrate a littttle bit
        Vector3 correction = normal * std::max(co.depth - pene, 0.0f) * percent
			* (1.0f / (1.0f / co.erb->mass + 1.0f / co.orb->mass));
		
		if (!co.erb->is_static) {
			co.etrans->position.subtract(correction * co.erb->mass);
		}
		if (!co.orb->is_static) {
			co.otrans->position.add(correction * co.orb->mass);
		}
    }

    m_collided_pairs.clear(); // clear the collision queue
}

// puts the collision normal and depth magnitude into the parameter references
void SCollision::GetCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
	switch (e.volume_type) {
		case AABB:
			if (o.volume_type == AABB) ::AABBCollisionInfo(e, o, normal, depth);
			else ::AABBvsSphereCollisionInfo(e, o, normal, depth);
			break;
		case SPHERE:
			if (o.volume_type == AABB) ::AABBvsSphereCollisionInfo(e, o, normal, depth);
			else ::SphereCollisionInfo(e, o, normal, depth);
			break;
		default:
			break;
	}
}
