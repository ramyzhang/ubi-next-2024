#include "stdafx.h"

#include "SBoids.h"

void SBoids::Init() {
	for (int i = 0; i < m_num_boids; i++) {
		m_boids.push_back(SEntityManager::Instance().AddEntity("boid"));
	}

	// init the fake boids that will rotate around the stars
	for (int i = 0; i < m_num_hint_boids * STARS; i++) {
		if (m_hint_boids.count((EntityID)(i % STARS)) > 0) {
			m_hint_boids[(EntityID)(i % STARS)].push_back(SEntityManager::Instance().AddEntity("boid"));
		}
		else {
			m_hint_boids.insert({ (EntityID)(i % STARS), {SEntityManager::Instance().AddEntity("boid")} });
		}
	}
}

void SBoids::Instantiate(const std::vector<EntityID>& _targets) {
	m_targets = _targets;

	// instantiate the fake boids that will rotate around the stars
	for (int i = 0; i < STARS; i++) {
		auto node = m_hint_boids.extract(i);
		node.key() = m_targets[i];

		m_hint_boids.insert(std::move(node));

		CTransform* star_trans = SEntityManager::Instance().GetComponent<CTransform>(m_targets[i]);
		for (EntityID& e : m_hint_boids[m_targets[i]]) {
			float dist = RandomFloat(4.0f, 6.0f, e);
			m_sspawner.InstantiateHintBoid(
				e,
				star_trans->position + Vector3(dist, dist, dist));
		}
	}
}

void SBoids::Update(const float deltaTime) {	
	// just instantiate the boids once per frame if a star was collected so it looks like particles!
	if (!m_create_boid_queue.empty() && m_boid_i < m_num_boids) {
		m_sspawner.InstantiateBoid(m_boids[m_boid_i], m_create_boid_queue.front());
		m_boid_i++;
		m_create_boid_queue.pop_front();
	}

	// update the positions of all the boids
	for (EntityID e : EntityView<CBoid>(SEntityManager::Instance())) {
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);
		CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(e);
		CBoid*		cboid = SEntityManager::Instance().GetComponent<CBoid>(e);
		
		cboid->forward = FindNextDirection(ctrans, cboid, crb, e);
		Vector3 next_velo = Lerp(crb->velocity.normalize(), cboid->forward, 0.8f);
		
		ctrans->rotation.add(Vector3(RandomFloat(0, 1.0f, e), RandomFloat(0, 1.0f, e), RandomFloat(0, 1.0f, e)) * 0.001f * deltaTime);
		crb->velocity = next_velo * m_speed * deltaTime;
		cboid->forward = next_velo.normalize();
	}

	// update the positions of all the fake boids
	for (auto& elem : m_hint_boids) {
		CTransform* star_trans = SEntityManager::Instance().GetComponent<CTransform>(elem.first);
		for (EntityID& hint_boid : elem.second) {
			CTransform* boid_trans = SEntityManager::Instance().GetComponent<CTransform>(hint_boid);

			float dist = RandomFloat(3.0f, 5.0f, hint_boid);
			float speed_p = RandomFloat(0.001f, 0.005f, hint_boid);
			float speed_y = RandomFloat(0.001f, 0.005f, hint_boid + 1);

			float new_pitch = boid_trans->rotation.x + speed_p * deltaTime;
			float new_yaw = boid_trans->rotation.y + speed_y * deltaTime;

			Vector3 new_rot = Vector3(new_pitch, new_yaw, 0);
			Vector3 target_pos = Matrix4x4().rotate(new_rot) * Vector3(dist, dist, dist) + star_trans->position;

			boid_trans->rotation = Lerp(boid_trans->rotation, new_rot, 0.7f);
			boid_trans->position = Lerp(boid_trans->position, target_pos, 0.7f);
		}
	}
}

void SBoids::Shutdown() {
	m_boid_i = 0;
	m_create_boid_queue.clear();
	m_boids.clear();
	m_targets.clear();
	m_hint_boids.clear();
}

void SBoids::OnNotify(Event event, std::vector<EntityID> entities) {
	if (event != STAR_COLLECTED) return;

	EntityID star_id = entities[0];
	CTransform* star_trans = SEntityManager::Instance().GetComponent<CTransform>(star_id);

	// queue the boid instantiation so we don't hold up whatever process is currently running
	for (int i = 0; i < m_num_boids / STARS; i++) {
		m_create_boid_queue.push_back(star_trans->position);
	}
	
	// delete the fake boids around this star
	for (EntityID& fake_boid : m_hint_boids[star_id]) {
		SEntityManager::Instance().RemoveEntity(fake_boid);
	}

	m_hint_boids.erase(star_id);

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
