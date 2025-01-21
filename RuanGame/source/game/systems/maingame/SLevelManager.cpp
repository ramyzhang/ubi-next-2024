#include "stdafx.h"

#include "SLevelManager.h"

void SLevelManager::Init(const float& _width, const float& _depth, const float& _height) {
	m_width = _width;
	m_depth = _depth;
	m_height = _height;
	
	m_platform_w = m_width / m_grid_w;
	m_platform_d = m_depth / m_grid_d;

	for (int i = 0; i < m_grid_w; i++) {
		for (int j = 0; j < m_grid_d; j++) {
			m_grid[i][j] = MAX_ENTITIES;
		}
	}
}

void SLevelManager::InitPlatforms() {
	for (int i = 0; i < m_num_platforms; i++) {
		m_platform_coords.push_back({ RandomInt(0, m_grid_w - 1), RandomInt(0, m_grid_d - 1) });
	} // reserve four for the star platforms

	std::vector<GridCoord> to_swap;
	for (GridCoord& gc : m_platform_coords) {
		if (m_grid[gc.first][gc.second] != MAX_ENTITIES) {
			m_num_platforms--;
			continue;
		}
		m_grid[gc.first][gc.second] = SEntityManager::Instance().AddEntity("platform");
		to_swap.push_back(gc);
	}

	// prune any repeated random coordinates
	m_platform_coords.swap(to_swap);

	// select four of the random coordinates for the stars to be on
	for (int i = 0; i < STARS; i++) {
		stars.push_back(SEntityManager::Instance().AddEntity("star"));
		m_star_coords.push_back(m_platform_coords[i]);
	}

	m_moving_platform = SEntityManager::Instance().AddEntity("moving");
}

std::vector<EntityID> SLevelManager::InstantiatePlatforms() {
	std::vector<EntityID> star_ids;
	for (int i = 0; i < STARS; i++) {
		GridCoord& gc = m_platform_coords[i];

		WorldCoord wc = ConvertToWorldCoords(gc);
		float y_scale = (float)i * 10.0f;
		if (y_scale == 0) y_scale = 5.0f;

		m_sspawner.InstantiateCube(
			m_grid[gc.first][gc.second],
			Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f)),
			Vector3(wc.first, -m_height / 2.0f + y_scale, wc.second),
			Vector3(m_platform_w / 2.0f, y_scale, m_platform_d / 2.0f),
			true
		);

		m_sspawner.InstantiateStar(
			stars[i],
			Vector3(wc.first, -m_height / 2.0f + y_scale * 2.0f + 5.0f, wc.second)
		);
		star_ids.push_back(stars[i]);
	}
	
	// instantiate the random platforms
	for (int i = STARS; i < m_platform_coords.size(); i++) {
		GridCoord& gc = m_platform_coords[i];

		if (m_grid[gc.first][gc.second] == MAX_ENTITIES) continue;

		WorldCoord wc = ConvertToWorldCoords(gc);
		float y_scale = RandomFloat(10.0f, 10.0f * (STARS - 1) - 5.0f);

		m_sspawner.InstantiateCube(
			m_grid[gc.first][gc.second],
			Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f)),
			Vector3(wc.first, -m_height / 2.0f + y_scale, wc.second),
			Vector3(m_platform_w / 2.0f, y_scale, m_platform_d / 2.0f),
			true);
	}

	// instantiate the moving platform
	m_sspawner.InstantiateCube(
		m_moving_platform,
		Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f)),
		Vector3(RandomFloat(0, m_width / 2.0f), 40.0f, RandomFloat(0, m_depth / 2.0f)),
		Vector3(10.0f, 10.0f, 20.0f),
		true);

	m_move_dir = Vector3(1.0f, 0, 0);

	return star_ids;
}

void SLevelManager::Update(const float deltaTime) {
	if (App::IsKeyPressed(VK_TAB)) m_game_won = true; // TODO: delete this	

	CTransform* move_trans = SEntityManager::Instance().GetComponent<CTransform>(m_moving_platform);
	if (std::abs(move_trans->position.x) > m_width / 2.0f) {
		m_move_dir = Vector3() - m_move_dir;
	}
	move_trans->position = Lerp(move_trans->position, move_trans->position + m_move_dir, 0.1f * deltaTime);
}

void SLevelManager::Shutdown() {
	m_moves = 0;
	m_score = 0;
	m_game_won = false;
	stars.clear();
	m_star_coords.clear();
	m_platform_coords.clear();
}

void SLevelManager::OnNotify(Event event, std::vector<EntityID> entities) {
	if (event != STAR_COLLECTED) return;

	m_score++;

	if (m_score == m_max_score) {
		App::PlaySound("data/music/you-win.wav", false);
		m_game_won = true;

		if (m_moves <= m_high_score) {
			m_high_score = m_moves;
		}
	}
	else {
		App::PlaySound("data/music/fish-freed.wav", false);
	}
}

WorldCoord SLevelManager::ConvertToWorldCoords(GridCoord _gc) const {
	float x = _gc.first * m_platform_w - m_width / 2.0f + m_platform_w / 2.0f;
	float z = _gc.second * m_platform_d - m_depth / 2.0f + m_platform_d / 2.0f;
	return { x, z };
}
