#include "stdafx.h"

#include "MainMenu.h"

void MainMenu::InitGOs() {
	m_title = SEntityManager::Instance().AddEntity("ui");
	m_play = SEntityManager::Instance().AddEntity("ui");
	m_quit = SEntityManager::Instance().AddEntity("ui");
	m_indicator = SEntityManager::Instance().AddEntity("ui");

	SEntityManager::Instance().Update(); // initial update to get the new entity in the list
}

void MainMenu::InstantiateGOs() {
	CUIText ctitle = {};
	ctitle.pos = Vector3(500.0f, 480.0f, 0.0f);
	ctitle.rgb = Vector3(0.3f, 0.8f, 0.44f);
	ctitle.text = "Ruan: The Game";
	SEntityManager::Instance().AddComponent<CUIText>(m_title, ctitle);

	CUIText cplay = {};
	cplay.pos = Vector3(500.0f, 410.0f, 0.0f);
	cplay.rgb = Vector3(0.3f, 0.8f, 0.44f);
	cplay.text = "play";
	SEntityManager::Instance().AddComponent<CUIText>(m_play, cplay);
	
	CUIText cquit = {};
	cquit.pos = Vector3(500.0f, 380.0f, 0.0f);
	cquit.rgb = Vector3(0.3f, 0.8f, 0.44f);
	cquit.text = "quit";
	SEntityManager::Instance().AddComponent<CUIText>(m_quit, cquit);
	
	CUIText cindic = {};
	cindic.pos = Vector3(450.0f, 410.0f, 0.0f);
	cindic.rgb = Vector3(0.8f, 0.67f, 0.31f);
	cindic.text = ">";
	SEntityManager::Instance().AddComponent<CUIText>(m_indicator, cindic);
}

void MainMenu::Update(const float deltaTime) {
}

void MainMenu::LateUpdate(const float deltaTime) {
}

void MainMenu::Shutdown() {
}
