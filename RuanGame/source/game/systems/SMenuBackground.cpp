#include "stdafx.h"
#include "SMenuBackground.h"

void SMenuBackground::Init(std::vector<EntityID>& dodecahedrons) {
	m_dodecahedrons = dodecahedrons;
}

void SMenuBackground::Update(const float deltaTime) {
	for (EntityID& e : m_dodecahedrons) {
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(e);
		Vector3 speed = Vector3(RandomFloat(0.0001f, 0.0005f, e), RandomFloat(0.0002f, 0.0003f, e), RandomFloat(0.0001f, 0.0002f, e));
		ctrans->rotation.add(speed.scale(deltaTime));
	}
}
