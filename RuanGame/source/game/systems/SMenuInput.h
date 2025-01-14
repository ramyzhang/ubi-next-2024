#pragma once

#include "engine/EngineIncludes.h"

class SMenuInput {
public:
	void Init(const EntityID& indic);
	// return false if we need to go to next level
	bool HandleInput(const float deltaTime);

private:
	EntityID		m_indicator;

	Vector3			m_play_pos = Vector3(450.0f, 410.0f, 0.0f);
	Vector3			m_quit_pos = Vector3(450.0f, 380.0f, 0.0f);

	bool			m_indic_play = true;
};