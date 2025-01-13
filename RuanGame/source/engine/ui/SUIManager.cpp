#include "stdafx.h"

#include "SUIManager.h"

void SUIManager::Init() {
}

void SUIManager::Update(const float deltaTime) {
	m_text_queue.clear();
	m_sprite_queue.clear();

	// set up render buffer such that farthest objects are drawn before closest objects
	for (EntityID e : EntityView<CUIText>(SEntityManager::Instance())) {
		CUIText* ctext = SEntityManager::Instance().GetComponent<CUIText>(e);
		m_text_queue.push_back(*ctext);
	}

	for (EntityID e : EntityView<CUISprite>(SEntityManager::Instance())) {
		CUISprite* csprite = SEntityManager::Instance().GetComponent<CUISprite>(e);
		m_sprite_queue.push_back(*csprite);
	}
}

void SUIManager::Render() {
	for (CUISprite& s : m_sprite_queue) {
		s.sprite->Draw();
	}

	for (CUIText& t : m_text_queue) {
		App::Print(t.pos.x, t.pos.y, t.text, t.rgb.x, t.rgb.y, t.rgb.z, m_font);
	}
}
