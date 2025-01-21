#include "stdafx.h"
#include "SMovement.h"

void SMovement::Init(const EntityID& _player, const EntityID& _ground, const float& _mass) {
	m_player = _player;
	m_ground = _ground;
	m_mass = _mass;

	for (int i = 0; i < path_length; i++) {
		path.push_back(SEntityManager::Instance().AddEntity("path"));
	}
}

void SMovement::Instantiate() {
	for (EntityID& e : path) {
		m_sspawner.InstantiatePathDodec(
			e,
			Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f)),
			Vector3(RandomFloat(-20.0f, 20.0f), 0, 0));
	}

	CMesh* cmesh = SEntityManager::Instance().GetComponent<CMesh>(path[0]);
	m_cached_path_mesh = *cmesh;
}

void SMovement::Update(const float deltaTime) {
	CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(m_player);
	CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(m_player);

	if (ctrans->position.y < -LEVEL_HEIGHT) ctrans->position = m_last_position;

	Vector3 new_force = Vector3(0, 0, 0);

	float pitch = SCamera::Instance().pitch;
	float yaw = SCamera::Instance().yaw;
	Vector3 new_rotation = Vector3(0, yaw + (float)M_PI, 0);
	Matrix4x4 rot_mat = Matrix4x4().rotate(new_rotation);

	ctrans->rotation = new_rotation;
	new_force = rot_mat * Vector3(0, 10.0f, 20.0f) * m_mass * m_multiplier;

	UpdatePath(ctrans->position, new_force);

	// check if player has collided with any platforms or floors
	if (m_collided_set.count(m_player) > 0) m_grounded = true;
	else m_grounded = false;
	
	if (App::IsKeyPressed(VK_SPACE) && !m_key_was_pressed && m_grounded) {
		SLevelManager::Instance().IncreaseMoves();
		int random = RandomInt(1, 3);
		if (random == 1) App::PlaySound("data/music/launch-1.wav");
		if (random == 2) App::PlaySound("data/music/launch-2.wav");
		if (random == 3) App::PlaySound("data/music/launch-3.wav");
		
		m_last_position = ctrans->position;
		crb->force = new_force * deltaTime;

		TogglePath();

		// m_grounded = false; // reset the grounded variable for the next frame
		m_key_was_pressed = true;
	}
	else if (m_grounded && !App::IsKeyPressed(VK_SPACE)) {
		TogglePath();
	}

	if (!App::IsKeyPressed(VK_SPACE)) m_key_was_pressed = false; // avoid re-triggering a ton

	m_collided_set.clear(); // reset the set of collided pairs
}

void SMovement::Shutdown() {
	FreeContainer(path);
}

void SMovement::TogglePath() {
	for (EntityID& e : path) {
		if (!m_grounded) {
			SEntityManager::Instance().RemoveComponent<CMesh>(e);
		}
		else {
			if (SEntityManager::Instance().GetEntity(e).CheckComponent(CMESH)) return;
			m_cached_path_mesh.colour = Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f));
			SEntityManager::Instance().AddComponent<CMesh>(e, m_cached_path_mesh);
		}
	}
}

void SMovement::UpdatePath(const Vector3& position, const Vector3& force) {\
	// at peak height, vertical velocity = 0
	// v = v0 + at
	// 0 = force.y + gravity * t
	float time_to_apex = -force.y / (m_gravity * m_multiplier);
	float total_time = time_to_apex * 2; // Time to go up and come back down

	float time_step = total_time / (path_length - 1);

	for (int i = 0; i < path_length; i++) {
		float t = time_step * i;
		if (i == 0) t = time_step * 0.5f; // don't want it being inside the player
		Vector3 new_position = position +
			Vector3(
				force.x * t,
				force.y * t + 0.5f * m_gravity * m_multiplier * t * t,
				force.z * t
			);

		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(path[i]);
		ctrans->position = new_position;
	}
}

void SMovement::OnNotify(Event event, std::vector<EntityID> entities) {
	if (event != COLLISION) return;

	std::string tag0 = SEntityManager::Instance().GetEntity(entities[0]).GetTag();
	std::string tag1 = SEntityManager::Instance().GetEntity(entities[1]).GetTag();

	if (tag0 == "player") {
		if (tag1 == "floor" || tag1 == "platform") {
			m_collided_set.insert(entities[0]);
			m_collided_set.insert(entities[1]);
		}
	}
	else if (tag1 == "player") {
		if (tag0 == "floor" || tag0 == "platform") {
			m_collided_set.insert(entities[0]);
			m_collided_set.insert(entities[1]);
		}
	}
}