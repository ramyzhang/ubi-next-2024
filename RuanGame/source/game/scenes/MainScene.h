#pragma once

#include "Scene.h"
#include "game\GameIncludes.h"

class MainScene : public Scene {

public:
	void InitGOs();
	void InstantiateGOs();
	void Update(const float deltaTime);
	void LateUpdate(const float deltaTime);
	void Shutdown();
};