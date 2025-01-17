#pragma once

#include "engine/EngineIncludes.h"
#include "App/AppSettings.h"

enum MenuResults {
	PLAY,
	QUIT,
	NOTHING
};

class SMenuInput {
public:
	void Init(const EntityID& indic);
	// return false if we need to go to next level
	MenuResults HandleInput(const float deltaTime);

	bool			indic_play = true;

private:
	EntityID		m_indicator;

	Vector3			m_play_pos = Vector3(450.0f, 410.0f, 0.0f);
	Vector3			m_quit_pos = Vector3(450.0f, 380.0f, 0.0f);

	bool			m_receive_input = true;
};