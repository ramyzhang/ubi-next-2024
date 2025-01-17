#include "stdafx.h"
#include "SMenuInput.h"

void SMenuInput::Init(const EntityID& indic) {
	m_indicator = indic;
}

MenuResults SMenuInput::HandleInput(const float deltaTime) {
	if (App::GetController().GetLeftThumbStickY() > 0.5f || App::GetController().GetLeftThumbStickY() < -0.5f) {
		CUIText* ctext = SEntityManager::Instance().GetComponent<CUIText>(m_indicator);

		if (indic_play && m_receive_input) {
			ctext->pos = m_quit_pos;
			indic_play = false;
			m_receive_input = false;
		}
		else if (!indic_play && m_receive_input) {
			ctext->pos = m_play_pos;
			indic_play = true;
			m_receive_input = false;
		}
	}
	else {
		m_receive_input = true;
	}

	if (App::GetController().CheckButton(XINPUT_GAMEPAD_START)) {
		if (indic_play) return PLAY;
		else return QUIT; // quit
	}

	return NOTHING;
}
