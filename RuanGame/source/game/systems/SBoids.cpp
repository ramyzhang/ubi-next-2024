#include "stdafx.h"

#include "SBoids.h"

void SBoids::Init() {
	CreateRays(); // only run this once
}

void SBoids::Update(const float deltaTime) {
	for (EntityID e : EntityView<CTransform, CRigidBody, CBoid>(SEntityManager::Instance())) {
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);
		CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(e);
		CBoid* cboid = SEntityManager::Instance().GetComponent<CBoid>(e);

		crb->velocity = FindNextDirection(ctrans, cboid, e) * m_speed;
	}
}

void SBoids::CreateRays() {
	float golden_ratio = (1.0f + sqrtf(5.0f)) / 2.0f;
	float big_phi = (float)M_PI * 2.0f * golden_ratio;

	for (int i = 0; i < m_num_points; i++) {
		Vector3 new_ray;

		float phi = acosf(1.0f - 2.0f * (float)i / (float)m_num_points);
		float theta = big_phi * (float)i;

		new_ray.x = cosf(theta) * sinf(phi);
		new_ray.y = sinf(theta) * sinf(phi);
		new_ray.z = cosf(phi);
		m_directions.push_back(new_ray);
	}
}

Vector3 SBoids::FindNextDirection(CTransform* ctrans, CBoid* cboid, const EntityID& e) {
	Vector3 ray_o = ctrans->position;
	Vector3 best_dir = cboid->forward; // set to current direction
	float furthest_dist = 0.0f;

	for (int i = 0; i < m_num_points; i++) {
		Vector3 ray_dir = m_directions[i] + ctrans->position;

		// are any of these rays colliding with other
		for (EntityID o : EntityView<CCollider>(SEntityManager::Instance())) {
			if (e == 0) continue;

			CCollider* ccollider = SEntityManager::Instance().GetComponent<CCollider>(o);

			bool hit = false;
			float dist;
			if (ccollider->volume_type == AABB) {
				hit = IntersectRayAABB(ray_o, ray_dir, dist, *ccollider);
			}
			else if (ccollider->volume_type == SPHERE) {
				hit = IntersectRaySphere(ray_o, ray_dir, dist, *ccollider);
			}

			if (hit) {
#ifdef _DEBUG // let's draw the hit ray
				SRenderer::Instance().DrawDebugLine(ray_o, ray_o + ray_dir.normalize() * dist);
#endif // _DEBUG
				if (dist > furthest_dist) {
					best_dir = ray_dir;
					furthest_dist = dist;
				}
			}
			else {
				return ray_dir; // just use this direction, since it's all clear
			}
		}
	}

	return best_dir;
}
