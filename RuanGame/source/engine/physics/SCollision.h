#pragma once

#include "core/System.h"
#include "engine/ecs/ecsutils/EntityView.h"
#include "core/Subject.h"

class SCollision : public System<SCollision>, public Subject {
public:
	void BroadUpdate(const float deltaTime); // really basic broad phase (just n^2 loop)
	void NarrowUpdate(const float deltaTime); // narrow phase here (collision resolution)

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

	bool IntersectRayCollider(const Vector3& ray_origin, Vector3 ray_dir, float& depth, const CCollider& ccol);
	bool IntersectRaySphere(const Vector3& ray_origin, Vector3 ray_dir, float& depth, const CCollider& sphere);
	bool IntersectRayAABB(const Vector3& ray_origin, const Vector3& ray_dir, float& depth, const CCollider& aabb);

private:
	void GetCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth);
	void SphereCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth);
	void AABBCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth);
	void AABBvsSphereCollisionInfo(const CCollider& e, const CCollider& o, Vector3& normal, float& depth);

	bool CheckCollision(const CCollider& e, const CCollider& other);
	bool AABBvsAABBCollision(const CCollider& a, const CCollider& b);
	bool SpherevsSphereCollision(const CCollider& a, const CCollider& b);
	bool AABBvsSphereCollision(const CCollider& box, const CCollider& sphere);

	std::vector<CollisionObject>	m_collided_pairs;
};

inline bool SCollision::AABBvsAABBCollision(const CCollider& a, const CCollider& b) {
	// typical aabb collision
	Vector3 minA = a.center - a.half_size;
	Vector3 maxA = a.center + a.half_size;
	Vector3 minB = b.center - b.half_size;
	Vector3 maxB = b.center + b.half_size;

	return (minA.x <= maxB.x && maxA.x >= minB.x) &&
		(minA.y <= maxB.y && maxA.y >= minB.y) &&
		(minA.z <= maxB.z && maxA.z >= minB.z);
}

inline bool SCollision::SpherevsSphereCollision(const CCollider& a, const CCollider& b) {
	// ezpz sphere collision
	float min_dist = a.radius + b.radius;
	return b.center.distance_squared(a.center) < min_dist * min_dist;
}

inline bool SCollision::AABBvsSphereCollision(const CCollider& box, const CCollider& sphere) {
	// find closest point on aabb to sphere center
	Vector3 center = sphere.center;
	Vector3 closest = center.clamp(box.center - box.half_size, box.center + box.half_size);

	return sphere.center.distance_squared(closest) < (sphere.radius * sphere.radius);
}