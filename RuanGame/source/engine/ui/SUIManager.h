#pragma once

#include <vector>
#include "core/System.h"
#include "engine/ecs/components/ComponentIncludes.h"
#include "engine/ecs/EntityView.h"

class SUIManager : public System<SUIManager> {
public:
	void Init();
	void Update(const float deltaTime);
	void Render();

private:
	std::vector<CUIText>	m_text_queue;

	void*					m_font = GLUT_BITMAP_8_BY_13;
};