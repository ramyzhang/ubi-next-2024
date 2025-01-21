#pragma once

#include "core/Observer.h"
#include "engine/EngineIncludes.h"
#include "game/GameConstants.h"
#include "game/systems/maingame/SSpawner.h"
#include "game/systems/maingame/SLevelManager.h"

class SMovement : public Observer, public Subject {
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
	int						path_length = 24;

private:
	void	TogglePath();
	void	UpdatePath(const Vector3& position, const Vector3& force);

	EntityID						m_player;
	EntityID						m_ground;
	std::unordered_set<EntityID>	m_collided_set;

	float		m_mass = 1.0f;
	float		m_min_multiplier = 1.0f;
	float		m_max_multiplier = 5.0f;
	float		m_multiplier = 1.0f;
	float		m_rotate_speed = 0.003f; // used in FREE mode and FIXEDTARGET mode

	bool		m_grounded = true;
	bool		m_key_was_pressed = false;

	float		m_gravity = -9.81f;

	Vector3		m_last_position;

	CMesh		m_cached_path_mesh;

	SSpawner	m_sspawner;
};