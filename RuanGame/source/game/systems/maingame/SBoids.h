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

class SBoids : public Observer {
public:
	void Init();
	void Instantiate();
	void Update(const float deltaTime);

	void OnNotify(Event event, std::vector<EntityID> entities);

private:
	Vector3		FindNextDirection(CTransform* ctrans, CBoid* cboid, CRigidBody* crb, const EntityID& e) const;
	Vector3		CheckAgainstBounds(Vector3 position) const;
	Vector3		GetAngle(Vector3 a, Vector3 b);

	std::vector<Vector3>	m_create_boid_queue;

	std::vector<EntityID>	m_boids; // TEMP

	int						m_num_boids = 64;

	float					m_radius = 30.0f; // radius of the boid
	float					m_speed = 0.3f;

	float					m_separation_weight = 1.8f;
	float					m_alignment_weight = 2.0f;
	float					m_cohesion_weight = 1.8f;
	float					m_bounding_weight = 0.7f;

	SSpawner				m_sspawner;
};