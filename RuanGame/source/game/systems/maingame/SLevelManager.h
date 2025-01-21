#pragma once

#include "math/Utils.h"
#include "game/systems/maingame/SSpawner.h"
#include "game/systems/maingame/SStars.h"
#include "game/GameConstants.h"

typedef std::pair<int, int>		GridCoord;
typedef std::pair<float, float> WorldCoord;

class SLevelManager : public System<SLevelManager>, public Observer {
public:
	void Init(const float& _width, const float& _depth, const float& _height);

	void InitPlatforms();
	std::vector<EntityID> InstantiatePlatforms();
	void Update(const float deltaTime);
	void Shutdown();

	std::vector<EntityID>	stars;

	void OnNotify(Event event, std::vector<EntityID> entities);

	bool IsGameWon() const { return m_game_won; };
	int	GetHighScoreMoves() const { return m_high_score; };
	int GetCurrentMoves() const { return m_moves; };
	void IncreaseMoves() { m_moves++; };

private:
	bool	m_game_won = false;
	int		m_max_score = 4;
	int		m_score = 0; // this is just for stars
	int		m_moves = 0;
	int		m_high_score = INT_MAX;

	WorldCoord	ConvertToWorldCoords(GridCoord _gc) const;

	float	m_width = 100.0f;
	float	m_depth = 80.0f;
	float	m_height = 50.0f;

	float	m_platform_w;
	float	m_platform_d;

	int		m_num_platforms = 20;
	float	m_max_platform_height = 40.0f;

	static const int	m_grid_w = 5;
	static const int	m_grid_d = 4;
	
	EntityID				m_moving_platform;
	EntityID				m_grid[m_grid_w][m_grid_d];
	std::vector<GridCoord>	m_star_coords;
	std::vector<GridCoord>	m_platform_coords;

	Vector3					m_move_dir;

	SSpawner	m_sspawner;
};