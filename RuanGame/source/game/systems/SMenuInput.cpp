#include "stdafx.h"
#include "SMenuInput.h"

void SMenuInput::Init(const EntityID& indic) {
	m_indicator = indic;
}

bool SMenuInput::HandleInput(const float deltaTime) {
	if (App::GetController().GetLeftThumbStickY() > 0.5f) {
		CUIText* ctext = SEntityManager::Instance().GetComponent<CUIText>(m_indicator);

		if (m_indic_play) {
			ctext->pos = m_quit_pos;
			m_indic_play = false;
		}
		else {
			ctext->pos = m_play_pos;
			m_indic_play = true;
		}
	}
	if (App::GetController().GetLeftThumbStickY() < -0.5f) {
		CUIText* ctext = SEntityManager::Instance().GetComponent<CUIText>(m_indicator);

		if (m_indic_play) {
			ctext->pos = m_quit_pos;
			m_indic_play = false;
		}
		else {
			ctext->pos = m_play_pos;
			m_indic_play = true;
		}
	}
	if (App::GetController().CheckButton(XINPUT_GAMEPAD_A)) {
		if (m_indic_play) return true;
	}
	return false;
}
