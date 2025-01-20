#pragma once

#include "core/Observer.h"
#include "engine/EngineIncludes.h"
#include "game/systems/maingame/SSpawner.h"
#include "game/GameConstants.h"

class SMovement : public Observer {
public:
	void Init(const EntityID& _player, const EntityID& _ground, const float& _mass);
	void Instantiate();
	void Update(const float deltaTime);
	void Shutdown();

	void SetMultiplier(const float& _ratio) {
		float portion = m_min_multiplier + (m_max_multiplier - m_min_multiplier) * _ratio;
		m_multiplier = std::clamp(portion, m_min_multiplier, m_max_multiplier);
	}

	void OnNotify(Event event, std::vector<EntityID> entities);

	std::vector<EntityID>	path;
	int						path_length = 12;

private:
	void	TogglePath();
	void	UpdatePath(const Vector3& position, const Vector3& force, const float& deltaTime);

	EntityID				m_player;
	EntityID				m_ground;

	float		m_mass = 1.0f;
	float		m_min_multiplier = 0.5f;
	float		m_max_multiplier = 2.0f;
	float		m_multiplier = 1.0f;
	float		m_rotate_speed = 0.003f; // used in FREE mode and FIXEDTARGET mode

	bool		m_grounded = true;

	float		m_gravity = -9.81f;

	Vector3		m_last_position;

	CMesh		m_cached_path_mesh;

	SSpawner	m_sspawner;
};