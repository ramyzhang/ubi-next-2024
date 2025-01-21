#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include "math/Vector3.h"
#include "math/Intersection.h"
#include "engine/ecs/ecsutils/EntityView.h"
#include "engine/EngineIncludes.h"
#include "game/GameConstants.h"
#include "game/systems/maingame/SSpawner.h"

class SBoids : public System<SBoids>, public Observer {
public:
	void Init();
	void Instantiate(const std::vector<EntityID>& _targets);
	void Update(const float deltaTime);
	void Shutdown();

	void OnNotify(Event event, std::vector<EntityID> entities);

private:
	Vector3		FindNextDirection(CTransform* ctrans, CBoid* cboid, CRigidBody* crb, const EntityID& e) const;
	Vector3		CheckAgainstBounds(Vector3 position) const;

	std::deque<Vector3>		m_create_boid_queue;

	std::vector<EntityID>									m_boids;
	std::vector<EntityID>									m_targets;
	std::unordered_map<EntityID, std::vector<EntityID>>		m_hint_boids;

	int						m_num_boids = 256;
	int						m_boid_i = 0;
	int						m_num_hint_boids = 4;

	float					m_radius = 20.0f; // radius of the boid
	float					m_speed = 0.5f;

	float					m_separation_weight = 1.8f;
	float					m_alignment_weight = 3.0f;
	float					m_cohesion_weight = 2.5f;
	float					m_bounding_weight = 0.7f;

	SSpawner				m_sspawner;
};