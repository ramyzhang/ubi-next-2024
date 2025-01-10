#pragma once

#include <cassert>
#include "core/System.h"
#include "graphics/SRenderer.h"
#include "ecs/SEntityManager.h"

// Engine class holds and runs all of the core functions that aren't gameplay-related
class Engine : public System<Engine> {
public:
	void CoreInit();
	void CoreUpdate(const float deltaTime);
	void CoreRender();
	void CoreShutdown();
};