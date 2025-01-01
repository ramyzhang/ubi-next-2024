#pragma once

#include <unordered_map>
#include "scenes/Scene.h"
#include "scenes/SceneInclude.h"

class Game {

public: 
	std::unordered_map<std::string, Scene*> scenes;
	std::string current_scene;

	void Init();
	void Update(const float deltaTime);
	void LateUpdate(const float deltaTime);
	void Shutdown();

};