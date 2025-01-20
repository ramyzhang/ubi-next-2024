#include "stdafx.h"

#include "SCollision.h"

void SCollision::BroadUpdate(const float deltaTime) {
	// just a n^2 loop... sadly
	for (EntityID e : EntityView<CTransform, CCollider>(SEntityManager::Instance())) {
		Entity e_e = SEntityManager::Instance().GetEntity(e);

		CCollider* ecollider = SEntityManager::Instance().GetComponent<CCollider>(e);

		for (EntityID o : EntityView<CTransform, CCollider>(SEntityManager::Instance())) {
			if (o == e) continue;

			Entity e_o = SEntityManager::Instance().GetEntity(o);
			if (e_e.CheckComponent(CBOID) || e_o.CheckComponent(CBOID)) continue; // I'll deal with the boids in SBoid
			// this is so not clean but sacrificing for performance lol

			CCollider* ocollider = SEntityManager::Instance().GetComponent<CCollider>(o);

			if (CheckCollision(*ecollider, *ocollider)) {
				// since it's going to intersect, clamp the velocity to the depth returned by the collision
				// erb->velocity.normalize().scale(depth);

				Notify(COLLISION, { e, o }); // notify all observers

				if (!SEntityManager::Instance().GetEntity(e).CheckComponent(CRIGIDBODY)
					|| !SEntityManager::Instance().GetEntity(o).CheckComponent(CRIGIDBODY)) {
					continue; // one of the entities has no rigidbody, so we don't need to resolve anything
				}

				CTransform* etrans = SEntityManager::Instance().GetComponent<CTransform>(e);
				CTransform* otrans = SEntityManager::Instance().GetComponent<CTransform>(o);
				CRigidBody* erb = SEntityManager::Instance().GetComponent<CRigidBody>(e);
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
			}
		}
	}
}

// solve the collision! :) just basic impulse
void SCollision::NarrowUpdate(const float deltaTime) {
    for (CollisionObject& co : m_collided_pairs) {
		if (co.depth < 0) continue; // just ignore the ones that aren't actually going to collide

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
			if (o.volume_type == AABB) AABBCollisionInfo(e, o, normal, depth);
			else AABBvsSphereCollisionInfo(e, o, normal, depth);
			break;
		case SPHERE:
			if (o.volume_type == AABB) AABBvsSphereCollisionInfo(e, o, normal, depth);
			else SphereCollisionInfo(e, o, normal, depth);
			break;
		default:
			break;
	}
}

void SCollision::SphereCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
	Vector3 diff = o.center - e.center; // get the distance between the centers
	float distance = diff.magnitude();
	normal = diff * (1.0f / distance); // that's the normal!
	depth = (e.radius + o.radius) - distance; // get the depth of penetration
}

void SCollision::AABBCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
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

void SCollision::AABBvsSphereCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth) {
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


bool SCollision::IntersectRayCollider(const Vector3& ray_origin, Vector3 ray_dir, float& depth, const CCollider& ccol) {
	switch (ccol.volume_type) {
		case AABB:
			return IntersectRayAABB(ray_origin, ray_dir, depth, ccol);
		case SPHERE:
			return IntersectRaySphere(ray_origin, ray_dir, depth, ccol);
		default:
			break;
	}

	return false;
}

// returns earliest intersection distance in the t variable - this is the analytic solution
bool SCollision::IntersectRaySphere(const Vector3& ray_origin, Vector3 ray_dir, float& depth, const CCollider& sphere) {
	Vector3 ray_norm = ray_dir.normalize();

	Vector3 dist = sphere.center - ray_origin;
	float closest = dist.dot(ray_norm);

	if (closest < 0) return false; // if dotprod is negative, then the ray points away from the sphere

	float dist2 = dist.dot(dist) - closest * closest; // squared distance
	if (dist2 > sphere.radius * sphere.radius) return false; // ray missed the sphere

	float half_chord = sqrtf(sphere.radius * sphere.radius - dist2);
	float t0 = closest - half_chord;
	float t1 = closest + half_chord;

	float length = ray_dir.magnitude();
	depth = (t0 < 0) ? t1 : t0; // return the closest non-negative intersection distance
	return (depth >= 0 && length >= depth);
}

// based on the ScratchAPixel math
bool SCollision::IntersectRayAABB(const Vector3& ray_origin, const Vector3& ray_dir, float& depth, const CCollider& aabb) {
	Vector3 min_aabb = aabb.center - aabb.half_size;
	Vector3 max_aabb = aabb.center + aabb.half_size;

	float min_t;
	float max_t;

	// let's try to avoid divide by 0 error
	// first, calculate intersections with the x plane (facing left and right)
	float t0x = (min_aabb.x - ray_origin.x) / (ray_dir.x != 0 ? ray_dir.x : 0.00001f);
	float t1x = (max_aabb.x - ray_origin.x) / (ray_dir.x != 0 ? ray_dir.x : 0.00001f);

	min_t = std::min(t0x, t1x); // if t1x or t1y is bigger, then our math doesn't work lol
	max_t = std::max(t0x, t1x);

	// then, calculate intersections with the y plane (facing bottom and top)
	float t0y = (min_aabb.y - ray_origin.y) / (ray_dir.y != 0 ? ray_dir.y : 0.00001f);
	float t1y = (max_aabb.y - ray_origin.y) / (ray_dir.y != 0 ? ray_dir.y : 0.00001f);

	// if 
	min_t = std::max(min_t, std::min(t0y, t1y));
	max_t = std::min(max_t, std::max(t0y, t1y));

	// then, calculate intersections with the z plan (facing front and back)
	float t0z = (min_aabb.z - ray_origin.z) / (ray_dir.z != 0 ? ray_dir.z : 0.00001f);
	float t1z = (max_aabb.z - ray_origin.z) / (ray_dir.z != 0 ? ray_dir.z : 0.00001f);

	min_t = std::max(min_t, std::min(t0z, t1z));
	max_t = std::min(max_t, std::max(t0z, t1z));

	depth = min_t;
	float length = ray_dir.magnitude();
	return (max_t >= min_t && max_t >= 0 && min_t <= length);
}

inline bool SCollision::CheckCollision(const CCollider& e, const CCollider& other) {
	bool res = false;

	switch (e.volume_type) {
	case AABB:
		if (other.volume_type == AABB) res = AABBvsAABBCollision(e, other);
		else if (other.volume_type == SPHERE) res = AABBvsSphereCollision(e, other);
		break;
	case SPHERE:
		if (other.volume_type == AABB) res = AABBvsSphereCollision(other, e);
		else if (other.volume_type == SPHERE) res = SpherevsSphereCollision(e, other);
		break;
	default:
		break;
	}

	return res;
}