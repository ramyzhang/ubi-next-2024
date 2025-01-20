#pragma once

#include "Scene.h"
#include "math/Utils.h"
#include "game/systems/maingame/SMovement.h"
#include "game/systems/maingame/SSpawner.h"
#include "game/systems/maingame/SBoids.h"
#include "game/systems/maingame/SMainUI.h"
#include "game/systems/maingame/SLevelManager.h"
#include "game/systems/maingame/SStars.h"

class MainScene : public Scene {

public:
	void InitGOs();
	void InstantiateGOs();
	void Update(const float deltaTime);
	void LateUpdate(const float deltaTime);
	void Shutdown();

private:
	EntityID				m_player;
	EntityID				m_floor;
	std::vector<EntityID>	m_bounds;

	Vector3					m_camera_distance = Vector3(0, 0, 30.0f);

	SMovement				m_smovement;
	SSpawner				m_sspawner;
	//SBoids				m_sboids;
	SMainUI					m_sui;
	SLevelManager			m_slevelmanager;
	SStars					m_sstars;

	float					m_width = 200.0f;
	float					m_depth = 160.0f;
	float					m_height = 100.0f;
	float					m_thickness = 1.0f;
};