#pragma once

#pragma once

#include "Scene.h"
#include "core/IncrementRate.h"
#include "game/systems/maingame/SBoids.h"
#include "game/systems/maingame/SLevelManager.h"

class YouWin : public Scene {

public:
	void InitGOs();
	void InstantiateGOs();
	void Update(const float deltaTime);
	void LateUpdate(const float deltaTime);
	void Shutdown();

private:
	std::vector<EntityID>	m_you_win;
	EntityID				m_restart;
	EntityID				m_this_score;
	EntityID				m_high_score;
	EntityID				m_new_high_score;

	bool					m_high_score_beat = false;

	int						m_repeats = 5;

	int						m_rate_counter = 0;
	int						m_rate = 16;

	Vector3					m_green = Vector3(0.3f, 0.8f, 0.44f);
	Vector3					m_yellow = Vector3(0.8f, 0.67f, 0.31f);
};