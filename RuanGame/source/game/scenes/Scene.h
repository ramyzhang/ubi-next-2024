#pragma once

#include "engine/EngineIncludes.h"

enum SceneState {
	SCENE_STAY,
	SCENE_NEXT
};

class Scene {

public:
	Scene() : state(SCENE_STAY), next_scene(nullptr) {};

	virtual void InitGOs() = 0;
	virtual void InstantiateGOs() = 0;
	virtual void Update(const float deltaTime) = 0;
	virtual void LateUpdate(const float deltaTime) = 0;
	virtual void Shutdown() = 0;

	void Init() {
		InitGOs();
		SEntityManager::Instance().Update(); // initial update to get the new entities in the list
		InstantiateGOs();
	}

	void SetState(SceneState new_state) {
		state = new_state;
	}

	SceneState state;
	const char* next_scene;

};