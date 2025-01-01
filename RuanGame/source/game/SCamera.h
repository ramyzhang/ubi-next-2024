#pragma once

#include "core\System.h"
#include "engine\graphics\SRenderer.h"

class SCamera : public System<SCamera> {
public:
	void Init();
	void Update(const float deltaTime);
};