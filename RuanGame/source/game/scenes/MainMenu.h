#pragma once

#include "Scene.h"
#include "engine\EngineIncludes.h"
#include "game/systems/SMenuBackground.h"
#include "game/systems/SMenuInput.h"

class MainMenu : public Scene {

public:
	void InitGOs();
	void InstantiateGOs();
	void Update(const float deltaTime);
	void LateUpdate(const float deltaTime);
	void Shutdown();

private:
	EntityID				m_title;
	EntityID				m_play;
	EntityID				m_quit;
	EntityID				m_indicator;

	EntityID				m_border_top;
	EntityID				m_border_bottom;
	std::vector<EntityID>	m_border_left;
	std::vector<EntityID>	m_border_right;

	int						m_length_horizontal = 125;
	int						m_length_vertical = 57;

	std::vector<EntityID>	m_dodecahedrons;

	int						m_num_dodecs = 10;

	SMenuBackground			m_sbackground;
	SMenuInput				m_sinput;
};