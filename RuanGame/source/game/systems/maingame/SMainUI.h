#pragma once

#include "engine/EngineIncludes.h"
#include "core/IncrementRate.h"
#include "game/GameConstants.h"
#include "game/systems/maingame/SLevelManager.h"

class SMainUI : public Observer {

public:
	void InitGUI();
	void InstantiateGUI() const;
	void Update(const float deltaTime);
	void Shutdown() const;

	float GetGageRatio() const { return (float)m_gage_level / (float)m_gage; };

	void OnNotify(Event event, std::vector<EntityID> entities);

private:
	int		m_gage = 10;
	int		m_gage_level = 0;
	int		m_rate_counter = 0;
	int		m_rate = 8;
	bool	m_decrementing = false;

	int		m_star_score = 0;
	int		m_moves_score = 0;

	EntityID	m_measures;
	EntityID	m_score_keeper;
	EntityID	m_move_keeper;

	Vector3		m_green = Vector3(0.3f, 0.8f, 0.44f);
	Vector3		m_yellow = Vector3(0.8f, 0.67f, 0.31f);
};