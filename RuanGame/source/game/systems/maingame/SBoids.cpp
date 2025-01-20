#include "stdafx.h"

#include "SBoids.h"

void SBoids::Init() {
	for (int i = 0; i < m_num_boids; i++) {
		m_boids.push_back(SEntityManager::Instance().AddEntity("boid"));
	}
}

void SBoids::Instantiate() {
	for (EntityID& e : m_boids) {
		m_sspawner.InstantiateBoid(e, Vector3());
	}
}

void SBoids::Update(const float deltaTime) {
	m_create_boid_queue.clear();
	
	for (EntityID e : EntityView<CBoid>(SEntityManager::Instance())) {
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);
		CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(e);
		CBoid* cboid = SEntityManager::Instance().GetComponent<CBoid>(e);
		
		cboid->forward = FindNextDirection(ctrans, cboid, crb, e);
		Vector3 next_velo = Lerp(crb->velocity.normalize(), cboid->forward, 0.8f);
		
		ctrans->rotation.add(Vector3(RandomFloat(0, 1.0f), RandomFloat(0, 1.0f), RandomFloat(0, 1.0f)) * 0.001f * deltaTime);

		crb->velocity = next_velo * m_speed * deltaTime;
		cboid->forward = next_velo.normalize();
	}
}

void SBoids::OnNotify(Event event, std::vector<EntityID> entities) {
	if (event != STAR_COLLECTED) return;

	EntityID star_id = entities[0];
	CTransform* star_trans = SEntityManager::Instance().GetComponent<CTransform>(star_id);

	m_create_boid_queue.push_back(star_trans->position);

	// delete the star
	SEntityManager::Instance().RemoveEntity(star_id);
}

Vector3 SBoids::FindNextDirection(CTransform* ctrans, CBoid* cboid, CRigidBody* crb, const EntityID& e) const {
	Vector3 avg_separation = Vector3();
	Vector3 avg_alignment = Vector3();
	Vector3 avg_cohesion = Vector3();

	std::vector<EntityID> nearby_boids;
	// check through all other boids and add to the nearby boids vector if
	// they're within m_radius and about a 300-degree viewing angle
	for (EntityID o : EntityView<CBoid>(SEntityManager::Instance())) {
		if (e == o) continue;

		CTransform* otrans = SEntityManager::Instance().GetComponent<CTransform>(o);
		CCollider* ocol = SEntityManager::Instance().GetComponent<CCollider>(o);

		const float dist = ctrans->position.distance(otrans->position);

		if (dist > m_radius) continue;
		if (ctrans->position.dot(cboid->forward) < 0.5f) continue;

		nearby_boids.push_back(o);
	}

	// iterate through the nearby boids and start making the mean steering direction
	if (nearby_boids.size() > 0) {
		float count = 0;
		for (EntityID& o : nearby_boids) {
			CTransform* otrans = SEntityManager::Instance().GetComponent<CTransform>(o);
			CRigidBody* orb = SEntityManager::Instance().GetComponent<CRigidBody>(o);
			CBoid* oboid = SEntityManager::Instance().GetComponent<CBoid>(o);
			
			Vector3 diff = otrans->position - ctrans->position;

			// add up the averages for each rule
			avg_cohesion.add((diff - avg_cohesion) / (count + 1.0f));
			avg_alignment.add((oboid->forward - avg_alignment) / (count + 1.0f));
			avg_separation.subtract(diff / diff.dot(diff));
			
			count += 1.0f;
		}

		avg_cohesion = avg_cohesion.normalize();
		avg_alignment = avg_alignment.normalize();
		avg_separation = avg_separation.normalize();
	}

	// weight by the rule weights and check against the bounds of the level now
	Vector3 sum = avg_separation * m_separation_weight +
		avg_alignment * m_alignment_weight +
		avg_cohesion * m_cohesion_weight + CheckAgainstBounds(ctrans->position);

	sum.normalize();
	
	if (sum.magnitude_squared() < 0.1f) {
		return cboid->forward;
	}
	return sum;
}

Vector3 SBoids::CheckAgainstBounds(Vector3 position) const {
	Vector3 avg_walls = Vector3();
	Vector3 e_pos = position;
	e_pos.abs();
	if (e_pos.x > LEVEL_WIDTH - m_radius) {
		float floor = -LEVEL_WIDTH;
		float ceil = LEVEL_WIDTH;

		float diff_floor = position.x - floor;
		if (diff_floor < m_radius) {
			avg_walls.x += std::abs(1.0f / diff_floor);
		}

		float diff_ceil = ceil - position.x;
		if (diff_ceil < m_radius) {
			avg_walls.x -= std::abs(1.0f / diff_ceil);
		}
	}
	if (position.y < -LEVEL_HEIGHT + m_radius || position.y > LEVEL_HEIGHT * 2 - m_radius) {
		float floor = -LEVEL_HEIGHT;
		float ceil = LEVEL_HEIGHT * 2;

		float diff_floor = position.y - floor;
		if (diff_floor < m_radius) {
			avg_walls.y += std::abs(1.0f / diff_floor);
		}

		float diff_ceil = ceil - position.y;
		if (diff_ceil < m_radius) {
			avg_walls.y -= std::abs(1.0f / diff_ceil);
		}
	}
	if (e_pos.z > LEVEL_DEPTH - m_radius) {
		float floor = -LEVEL_DEPTH;
		float ceil = LEVEL_DEPTH;

		float diff_floor = position.z - floor;
		if (diff_floor < m_radius) {
			avg_walls.z += std::abs(1.0f / diff_floor);
		}

		float diff_ceil = ceil - position.z;
		if (diff_ceil < m_radius) {
			avg_walls.z -= std::abs(1.0f / diff_ceil);
		}
	}

	avg_walls.normalize();

	return avg_walls;
}

Vector3 SBoids::GetAngle(Vector3 a, Vector3 b) {
	a.normalize();
	b.normalize();

	// get axis of rotation via cross product
	Vector3 rot_axis = a.cross(b);
	rot_axis = rot_axis.normalize();

	float d = std::acosf(a.dot(b));

	if (rot_axis.magnitude() < 0.001) {
		return Vector3(0, 0, 0);
	}

	Vector3 angle;
	angle.x = std::atan2f(rot_axis.y, rot_axis.z) * d; // pitch
	angle.y = std::atan2f(rot_axis.x, rot_axis.z) * d; // yaw
	angle.z = std::atan2f(rot_axis.x, rot_axis.y) * d; // roll

	return angle;
}

