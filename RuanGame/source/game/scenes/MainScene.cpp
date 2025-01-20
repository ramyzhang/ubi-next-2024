#include "stdafx.h"

#include "MainScene.h"

void MainScene::InitGOs() {
	m_smovement = SMovement();
	m_sspawner = SSpawner();
	// m_sboids = SBoids();
	m_sui = SMainUI();
	m_slevelmanager = SLevelManager();
	m_sstars = SStars();

	m_player = SEntityManager::Instance().AddEntity("player");
	m_floor = SEntityManager::Instance().AddEntity("floor");

	// invisible walls for the level
	for (int i = 0; i < 5; i++) {
		m_bounds.push_back(SEntityManager::Instance().AddEntity("bound"));
	}

	// spawning the path indicators
	m_smovement.Init(m_player, m_floor, 1.0f); // init the input handler
	// m_sboids.Init();
	m_sui.InitGUI();

	m_slevelmanager.Init(m_width, m_depth, m_height);
	m_slevelmanager.InitPlatforms();

	SCamera::Instance().mode = SCamera::FIXEDTARGET;
	SCamera::Instance().SetTarget(m_player);
	SCamera::Instance().SetTargetDistance(m_camera_distance);
	
	SRenderer::Instance().SetFloorEntity(m_floor);
}

void MainScene::InstantiateGOs() {
	// instantiate the walls and the floor
	m_sspawner.InstantiateFloor(
		m_floor,
		Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f)),
		Vector3(0, -m_height / 2.0f, 0),
		Vector3(m_width / 2.0f, m_thickness, m_depth / 2.0f));

	m_sspawner.InstantiateBound(m_bounds[0], Vector3(0, 0, -m_depth / 2.0f), Vector3(m_width / 2.0f, m_height / 2.0f, m_thickness)); // front face
	m_sspawner.InstantiateBound(m_bounds[1], Vector3(0, 0, m_depth / 2.0f), Vector3(m_width / 2.0f, m_height / 2.0f, m_thickness)); // back face
	m_sspawner.InstantiateBound(m_bounds[2], Vector3(m_width / 2.0f, 0, 0), Vector3(m_thickness, m_height / 2.0f, m_depth / 2.0f)); // east face
	m_sspawner.InstantiateBound(m_bounds[3], Vector3(-m_width / 2.0f, 0, 0), Vector3(m_thickness, m_height / 2.0f, m_depth / 2.0f)); // west face
	m_sspawner.InstantiateBound(m_bounds[4], Vector3(0, m_height / 2.0f, 0), Vector3(m_width / 2.0f, m_thickness, m_depth / 2.0f)); // top face

	m_sspawner.InstantiatePlayer(m_player);

	m_slevelmanager.InstantiatePlatforms();
	m_smovement.Instantiate();
	// m_sboids.Instantiate();

	m_sui.InstantiateGUI();

	m_sstars.Init(&m_slevelmanager.stars, m_player, m_slevelmanager.num_stars);

	// register observers for notifications
	SCollision::Instance().AddObserver(&m_smovement);
	SCollision::Instance().AddObserver(&m_sstars);
	m_sstars.AddObserver(&m_slevelmanager);
	m_sstars.AddObserver(&m_sui);
	// m_sstars.AddObserver(&m_sboids);

	App::PlaySound("data/music/water-game.wav", true); // loop
}

void MainScene::Update(const float deltaTime) {
	if (m_slevelmanager.IsGameWon()) SetState(SCENE_NEXT); // go to "you win" scene

	m_slevelmanager.Update(deltaTime);
	m_sui.Update(deltaTime);
}

void MainScene::LateUpdate(const float deltaTime) {
	// m_sboids.Update(deltaTime);
	m_smovement.SetMultiplier(m_sui.GetGageRatio());
	m_smovement.Update(deltaTime);

	m_sstars.Update(deltaTime);
}

void MainScene::Shutdown() {
	App::StopSound("data/music/water-game.wav");
	SCollision::Instance().RemoveObserver(&m_smovement);
	SCollision::Instance().RemoveObserver(&m_sstars);
	m_sstars.RemoveObserver(&m_slevelmanager);
	m_sstars.RemoveObserver(&m_sui);

	m_slevelmanager.Shutdown();
	m_smovement.Shutdown();
	m_sui.Shutdown();
}