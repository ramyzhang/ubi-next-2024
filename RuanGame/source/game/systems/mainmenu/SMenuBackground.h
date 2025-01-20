#pragma once

#include <vector>
#include "engine/EngineIncludes.h"
#include "math/Utils.h"

class SMenuBackground {
public:
	void Init(std::vector<EntityID>& dodecahedrons);
	void Update(const float deltaTime);

private:
	std::vector<EntityID>	m_dodecahedrons;
};