#include "stdafx.h"

#include "Game.h"

void Game::Init() {
	scenes.insert_or_assign("MainMenu", new MainMenu);
	scenes.insert_or_assign("MainScene", new MainScene);
	scenes.insert_or_assign("YouWin", new YouWin);
	scenes["MainMenu"]->next_scene = "MainScene";
	scenes["MainScene"]->next_scene = "YouWin";
	scenes["YouWin"]->next_scene = "MainMenu";

	current_scene = "MainMenu";
	scenes[current_scene]->Init();
};

void Game::Update(const float deltaTime) {
	scenes[current_scene]->Update(deltaTime);
	scenes[current_scene]->LateUpdate(deltaTime);
};

void Game::Render() {
	// update scene if desired
	if (scenes[current_scene]->state != SCENE_STAY) {
		std::string next;

		if (scenes.count(scenes[current_scene]->next_scene) <= 0) {
			OutputDebugStringA("Couldn't find the requested next scene!\n");
			return;
		}

		next = scenes[current_scene]->next_scene;
		scenes[current_scene]->Shutdown();
		scenes[current_scene]->state = SCENE_STAY;
		current_scene = next;

		scenes[current_scene]->Init();
	}
}

void Game::Shutdown() {
	scenes[current_scene]->Shutdown();

	// delete memory allocated for scenes
	for (auto& pair : scenes)
	{
		delete pair.second;
		pair.second = nullptr;
	}

	scenes.clear();
}