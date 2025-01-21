#include "stdafx.h"

#include "YouWin.h"

void YouWin::InitGOs() {
	SEntityManager::Instance().Update(); // refresh the entities

	for (int i = 0; i < m_repeats; i++) {
		m_you_win.push_back(SEntityManager::Instance().AddEntity("ui"));
	}
	m_restart = SEntityManager::Instance().AddEntity("ui");

	m_this_score = SEntityManager::Instance().AddEntity("ui");
	m_high_score = SEntityManager::Instance().AddEntity("ui");
	m_new_high_score = SEntityManager::Instance().AddEntity("ui");

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

	CUIText cscore = {};
	cscore.pos = Vector3(425.0f, 300.0f, 0.0f);
	cscore.text = "YOUR SCORE - " + std::to_string(SLevelManager::Instance().GetCurrentMoves()) + " MOVES";
	cscore.rgb = m_yellow;
	SEntityManager::Instance().AddComponent<CUIText>(m_this_score, cscore);
	
	CUIText chighscore = {};
	chighscore.pos = Vector3(425.0f, 280.0f, 0.0f);
	int high_score = SLevelManager::Instance().GetHighScoreMoves() == INT_MAX ? 0 : SLevelManager::Instance().GetHighScoreMoves();
	chighscore.text = "HIGH SCORE - " + std::to_string(high_score) + " MOVES";
	chighscore.rgb = m_green;
	SEntityManager::Instance().AddComponent<CUIText>(m_high_score, chighscore);

	CUIText crestart = {};
	crestart.pos = Vector3(450.0f, 200.0f, 0.0f);
	crestart.text = "[R] PLAY AGAIN";
	crestart.rgb = Vector3(1.0f, 1.0f, 1.0f);
	SEntityManager::Instance().AddComponent<CUIText>(m_restart, crestart);

	if (!App::IsSoundPlaying("data/music/water-menu.wav")) App::PlaySound("data/music/water-menu.wav", true); // loop

	if (SLevelManager::Instance().GetCurrentMoves() > SLevelManager::Instance().GetHighScoreMoves()) return;
	
	CUIText cbeat = {};
	cbeat.pos = Vector3(410.0f, 260.0f, 0.0f);
	cbeat.text = "!!! NEW PERSONAL BEST !!!";
	cbeat.rgb = m_yellow;
	SEntityManager::Instance().AddComponent<CUIText>(m_new_high_score, cbeat);
}

void YouWin::Update(const float deltaTime) {
	SLevelManager::Instance().Update(deltaTime);

	if (App::IsKeyPressed(0x52)) SetState(SCENE_NEXT);
	if (!IncrementRate(m_rate_counter, m_rate)) return;
	for (int i = 0; i < m_repeats; i++) {
		CUIText* ctext = SEntityManager::Instance().GetComponent<CUIText>(m_you_win[i]);
		ctext->rgb = (ctext->rgb.x == m_green.x) ? m_yellow : m_green;
	}
}

void YouWin::LateUpdate(const float deltaTime) {
	SBoids::Instance().Update(deltaTime);
}

void YouWin::Shutdown() {
	SBoids::Instance().Shutdown();
	SLevelManager::Instance().Shutdown();

	m_you_win.clear();
	m_high_score_beat = false;
	EntityID temp_e = (EntityID)MAX_ENTITIES;
	SRenderer::Instance().SetFloorEntity(temp_e);
	SEntityManager::Instance().ClearAllEntities(); // clear everything and go to main menu after
}