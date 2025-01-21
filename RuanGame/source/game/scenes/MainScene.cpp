#include "stdafx.h"

#include "MainScene.h"

void MainScene::InitGOs() {
	m_smovement = SMovement();
	m_sspawner = SSpawner();
	// m_sboids = SBoids();
	m_sui = SMainUI();
	// m_slevelmanager = SLevelManager();
	m_sstars = SStars();

	m_player = SEntityManager::Instance().AddEntity("player");
	m_floor = SEntityManager::Instance().AddEntity("floor");

	// invisible walls for the level
	for (int i = 0; i < 5; i++) {
		m_bounds.push_back(SEntityManager::Instance().AddEntity("bound"));
	}

	// spawning the path indicators
	m_smovement.Init(m_player, m_floor, 1.0f); // init the input handler
	SBoids::Instance().Init();
	m_sui.InitGUI();

	SLevelManager::Instance().Init(m_width, m_depth, m_height);
	SLevelManager::Instance().InitPlatforms();

	SCamera::Instance().mode = SCamera::FIXEDTARGET;
	SCamera::Instance().SetTarget(m_player);
	SCamera::Instance().SetTargetDistance(m_camera_distance);
	
	SRenderer::Instance().SetFloorEntity(m_floor);
}

void MainScene::InstantiateGOs() {
	// instantiate the walls and the floor
	m_sspawner.InstantiateFloor(
		m_floor,
		Vector3(RandomFloat(0.0f, 0.8f), RandomFloat(0.0f, 0.5f), RandomFloat(0.0f, 0.5f)),
		Vector3(0, -m_height / 2.0f, 0),
		Vector3(m_width / 2.0f, m_thickness, m_depth / 2.0f));

	m_sspawner.InstantiateBound(m_bounds[0], Vector3(0, 0, -m_depth / 2.0f), Vector3(m_width / 2.0f, m_height / 2.0f, m_thickness)); // front face
	m_sspawner.InstantiateBound(m_bounds[1], Vector3(0, 0, m_depth / 2.0f), Vector3(m_width / 2.0f, m_height / 2.0f, m_thickness)); // back face
	m_sspawner.InstantiateBound(m_bounds[2], Vector3(m_width / 2.0f, 0, 0), Vector3(m_thickness, m_height / 2.0f, m_depth / 2.0f)); // east face
	m_sspawner.InstantiateBound(m_bounds[3], Vector3(-m_width / 2.0f, 0, 0), Vector3(m_thickness, m_height / 2.0f, m_depth / 2.0f)); // west face
	m_sspawner.InstantiateBound(m_bounds[4], Vector3(0, m_height / 2.0f, 0), Vector3(m_width / 2.0f, m_thickness, m_depth / 2.0f)); // top face

	m_sspawner.InstantiatePlayer(m_player);

	std::vector<EntityID> star_ids = SLevelManager::Instance().InstantiatePlatforms();
	m_smovement.Instantiate();
	m_sstars.Init(&SLevelManager::Instance().stars, m_player, STARS);
	SBoids::Instance().Instantiate(star_ids);

	m_sui.InstantiateGUI();

	// register observers for notifications
	SCollision::Instance().AddObserver(&m_smovement);
	SCollision::Instance().AddObserver(&m_sstars);
	m_sstars.AddObserver(&SLevelManager::Instance());
	m_sstars.AddObserver(&m_sui);
	m_sstars.AddObserver(&SBoids::Instance());

	App::PlaySound("data/music/water-game.wav", true); // loop
}

void MainScene::Update(const float deltaTime) {
	if (SLevelManager::Instance().IsGameWon()) SetState(SCENE_NEXT); // go to "you win" scene

	SLevelManager::Instance().Update(deltaTime);
	m_sui.Update(deltaTime);
}

void MainScene::LateUpdate(const float deltaTime) {
	SBoids::Instance().Update(deltaTime);
	m_smovement.SetMultiplier(m_sui.GetGageRatio());
	m_smovement.Update(deltaTime);
	m_sstars.Update(deltaTime);
}

void MainScene::Shutdown() {
	App::StopSound("data/music/water-game.wav");
	SCollision::Instance().RemoveObserver(&m_smovement);
	SCollision::Instance().RemoveObserver(&m_sstars);
	m_sstars.RemoveObserver(&SLevelManager::Instance());
	m_sstars.RemoveObserver(&m_sui);
	m_sstars.RemoveObserver(&SBoids::Instance());

	m_smovement.Shutdown();
	m_sui.Shutdown();
}