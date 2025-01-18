#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include "math/Vector3.h"
#include "math/Intersection.h"
#include "engine/ecs/ecsutils/EntityView.h"
#include "engine/EngineIncludes.h"

class SBoids {
public:
	void Init();
	void Update(const float deltaTime);

	

private:
	void		CreateRays();
	Vector3		FindNextDirection(CTransform* ctrans, CBoid* cboid, const EntityID& e);
	int						m_num_points = 256;
	float					m_turn_fraction;
	std::vector<Vector3>	m_directions;
	float					m_speed;
};