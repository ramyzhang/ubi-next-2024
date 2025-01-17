//------------------------------------------------------------------------
#include "stdafx.h"
//------------------------------------------------------------------------
#include <windows.h> 
#include <math.h>  
//------------------------------------------------------------------------
#include "app\app.h"
#include "engine\Engine.h"
#include "game/Game.h"
//------------------------------------------------------------------------

Engine engine;
Game game;

//------------------------------------------------------------------------
// Define functions in API
//------------------------------------------------------------------------
void Init() {
	engine.CoreInit();

	game.Init();
}

void Update(const float deltaTime) {
	engine.CoreUpdate(deltaTime);

	game.Update(deltaTime);
}

void Render() {
	engine.CoreRender();

	game.Render();

	// TODO: gamerender()
}

void Shutdown() {
	// TODO: gameshutdown();

	// engine.CoreShutdown();
}