#pragma once

#include <cassert>
#include "graphics/SRenderer.h"

// Engine class holds and runs all of the core functions that aren't gameplay-related
class Engine {

public:
	// Grab the instance of the Engine singleton
	Engine() {
		assert(!m_instantiated);
		m_instantiated = true;
	};

	~Engine() {
		m_instantiated = false;
	};

	void CoreInit();
	void CoreUpdate(const float deltaTime);
	void CoreRender();
	// void CoreShutdown();

private:
	static bool m_instantiated;

};