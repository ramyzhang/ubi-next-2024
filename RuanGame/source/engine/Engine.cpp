#include "stdafx.h"

#include "Engine.h"

bool Engine::m_instantiated = false; // define the static variable

void Engine::CoreInit() {
	SRenderer::Instance().Init();
}

void Engine::CoreUpdate(const float deltaTime) {
	SRenderer::Instance().Update(deltaTime);
}

void Engine::CoreRender() {
	SRenderer::Instance().Render();
}

