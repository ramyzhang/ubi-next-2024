#include "stdafx.h"

#include "SUIManager.h"

void SUIManager::Init() {
}

void SUIManager::Update(const float deltaTime) {
	m_text_queue.clear();

	// set up render buffer such that farthest objects are drawn before closest objects
	for (EntityID e : EntityView<CUIText>(SEntityManager::Instance())) {
		CUIText* ctext = SEntityManager::Instance().GetComponent<CUIText>(e);
		m_text_queue.push_back(*ctext);
	}
}

void SUIManager::Render() {
	for (CUIText& t : m_text_queue) {
		App::Print(t.pos.x, t.pos.y, t.text.c_str(), t.rgb.x, t.rgb.y, t.rgb.z, m_font);
	}
}
