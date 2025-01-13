#pragma once

#include "Scene.h"
#include "engine\EngineIncludes.h"

class MainMenu : public Scene {

public:
	void InitGOs();
	void InstantiateGOs();
	void Update(const float deltaTime);
	void LateUpdate(const float deltaTime);
	void Shutdown();

private:
	EntityID	m_title;
	EntityID	m_play;
	EntityID	m_quit;
	EntityID	m_indicator;
};