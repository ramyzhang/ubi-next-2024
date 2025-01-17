#pragma once

#include "Scene.h"
#include "engine/EngineIncludes.h"
#include "math/Utils.h"
#include "game/systems/SMainInput.h"

class MainScene : public Scene {

public:
	void InitGOs();
	void InstantiateGOs();
	void Update(const float deltaTime);
	void LateUpdate(const float deltaTime);
	void Shutdown();

private:
	EntityID	m_player;
	Vector3		m_camera_distance = Vector3(0, 0, 20.0f);

	SMainInput	m_sinput;
};