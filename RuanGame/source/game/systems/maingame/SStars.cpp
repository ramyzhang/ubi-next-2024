#include "stdafx.h"
#include "SStars.h"

void SStars::Init(std::vector<EntityID>* _stars, EntityID _player, int _num_stars) {
	m_stars = _stars; m_num_stars = _num_stars;
	m_player = _player;
}

void SStars::Update(const float deltaTime) {
	for (int i = 0; i < m_stars->size(); i++) {
		CTransform* ctrans = SEntityManager::Instance().GetComponent<CTransform>(m_stars->at(i));

		ctrans->rotation.add(Vector3(0, m_rotation_speed * deltaTime, 0));
	}
}

void SStars::OnNotify(Event event, std::vector<EntityID> entities) {
	if (event != COLLISION) return;

	bool player = false;
	bool star = false;
	EntityID star_id = MAX_ENTITIES;
	for (EntityID& e : entities) {
		if (e == m_player) player = true;
		if (SEntityManager::Instance().GetEntity(e).GetTag() == "star") {
			star = true;
			star_id = e;
		}
	}

	if (player && star) {
		m_stars->erase(std::find(m_stars->begin(), m_stars->end(), star_id));

		Notify(STAR_COLLECTED, { m_player });

		SEntityManager::Instance().RemoveEntity(star_id);
	}
}
