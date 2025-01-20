#include "stdafx.h"

#include "YouWin.h"

void YouWin::InitGOs() {
	SEntityManager::Instance().Update(); // refresh the entities

	for (int i = 0; i < m_repeats; i++) {
		m_you_win.push_back(SEntityManager::Instance().AddEntity("ui"));
	}
	m_restart = SEntityManager::Instance().AddEntity("ui");
	SCamera::Instance().mode = SCamera::FIXEDTARGETROTATE;
}

void YouWin::InstantiateGOs() {
	std::string winning = "= 0 v 0 =    V I C T O R Y    = 0 v 0 =";
	
	for (int i = 0; i < m_repeats; i++) {
		CUIText ctitle = {};
		ctitle.text = winning;
		ctitle.pos = Vector3(360.0f, 600.0f - i * 20.0f, 0.0f);
		ctitle.rgb = (i % 2 == 0) ? m_green : m_yellow;
		SEntityManager::Instance().AddComponent<CUIText>(m_you_win[i], ctitle);
	}

	CUIText restart = {};
	restart.pos = Vector3(450.0f, 300.0f, 0.0f);
	restart.text = "[R] PLAY AGAIN";
	restart.rgb = Vector3(1.0f, 1.0f, 1.0f);
	SEntityManager::Instance().AddComponent<CUIText>(m_restart, restart);

	if (!App::IsSoundPlaying("data/music/water-menu.wav")) App::PlaySound("data/music/water-menu.wav", true); // loop
}

void YouWin::Update(const float deltaTime) {
	if (App::IsKeyPressed(0x52)) SetState(SCENE_NEXT);
	if (!IncrementRate(m_rate_counter, m_rate)) return;
	for (int i = 0; i < m_repeats; i++) {
		CUIText* ctext = SEntityManager::Instance().GetComponent<CUIText>(m_you_win[i]);
		ctext->rgb = (ctext->rgb.x == m_green.x) ? m_yellow : m_green;
	}
}

void YouWin::LateUpdate(const float deltaTime) {
}

void YouWin::Shutdown() {
	m_you_win.clear();
	EntityID temp_e = (EntityID)MAX_ENTITIES;
	SRenderer::Instance().SetFloorEntity(temp_e);
	SEntityManager::Instance().ClearAllEntities(); // clear everything and go to main menu after
}