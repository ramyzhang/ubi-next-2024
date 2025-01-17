#pragma once

#include "engine/EngineIncludes.h"

class SMainInput {
public:
	void Init(const EntityID& _player) { m_player = _player; };
	void HandleInput(const float deltaTime);

private:
	EntityID	m_player;
	float		m_speed = 0.05f;
};