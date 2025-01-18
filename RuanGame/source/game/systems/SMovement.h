#pragma once

#include "engine/EngineIncludes.h"

class SMovement {
public:
	void Init(const EntityID& _player, float _mass) { m_player = _player; m_mass = _mass; };
	void Update(const float deltaTime);

private:
	EntityID	m_player;
	float		m_mass = 1.0f;
	float		m_speed = 0.5f;
};