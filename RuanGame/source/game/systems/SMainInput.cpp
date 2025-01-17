#include "stdafx.h"
#include "SMainInput.h"

void SMainInput::HandleInput(const float deltaTime) {
	CRigidBody* crb = SEntityManager::Instance().GetComponent<CRigidBody>(m_player);

	Vector3 inputs = Vector3(0, 0, 0);
	if (App::GetController().GetLeftThumbStickX() > 0.5f) {
		inputs.add(Vector3(1.0f, 0, 0) * m_speed * deltaTime);
	}
	if (App::GetController().GetLeftThumbStickX() < -0.5f) {
		inputs.subtract(Vector3(1.0f, 0, 0) * m_speed * deltaTime);
	}
	if (App::GetController().GetLeftThumbStickY() > 0.5f) {
		inputs.add(Vector3(0, 1.0f, 0) * m_speed * deltaTime);
	}
	if (App::GetController().GetLeftThumbStickY() < -0.5f) {
		inputs.subtract(Vector3(0, 1.0f, 0) * m_speed * deltaTime);
	}

	crb->velocity = inputs;
}
