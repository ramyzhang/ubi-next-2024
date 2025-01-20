#pragma once

#include "math/Utils.h"
#include "game/systems/maingame/SSpawner.h"
#include "game/systems/maingame/SStars.h"

typedef std::pair<int, int>		GridCoord;
typedef std::pair<float, float> WorldCoord;

class SLevelManager : public Observer {
public:
	void Init(const float& _width, const float& _depth, const float& _height);

	void InitPlatforms();
	void InstantiatePlatforms();
	void Update(const float deltaTime);
	void Shutdown();

	static const int		num_stars = 4;
	std::vector<EntityID>	stars;

	void OnNotify(Event event, std::vector<EntityID> entities);

	bool IsGameWon() const { return m_game_won; };

private:
	bool	m_game_won = false;
	int		m_max_score = 4;
	int		m_score = 0;

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