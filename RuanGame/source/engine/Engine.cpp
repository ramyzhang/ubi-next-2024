#include "stdafx.h"

#include "Engine.h"

void Engine::CoreInit() {
	SEntityManager::Instance().Init();
	SRenderer::Instance().Init();
}

void Engine::CoreUpdate(const float deltaTime) {
	SEntityManager::Instance().Update();
	SPhysics::Instance().Update(deltaTime);
	SRenderer::Instance().Update(deltaTime);
	SUIManager::Instance().Update(deltaTime);
}

void Engine::CoreRender() {
	SRenderer::Instance().Render();
	SUIManager::Instance().Render();
}

void Engine::CoreShutdown() {
	SEntityManager::Instance().Shutdown();
}