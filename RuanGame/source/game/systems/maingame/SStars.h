#pragma once

#include <algorithm>
#include "engine/EngineIncludes.h"

class SStars : public Observer, public Subject {
public:
	void Init(std::vector<EntityID>* _stars, EntityID _player, int _num_stars);
	void Update(const float deltaTime);

	void OnNotify(Event event, std::vector<EntityID> entities);

private:
	EntityID				m_player;

	float					m_rotation_speed = 0.005f;
	int						m_num_stars;
	std::vector<EntityID>*	m_stars; // going to store a pointer, since i want to share it with SLevelManager
};