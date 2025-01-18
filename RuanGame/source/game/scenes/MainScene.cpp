#include "stdafx.h"

#include "MainScene.h"

void MainScene::InitGOs() {
	m_player = SEntityManager::Instance().AddEntity("player");

	m_floor = SEntityManager::Instance().AddEntity("floor");

	// boid boid boid boid boid
	m_boid = SEntityManager::Instance().AddEntity("boid");

	// TODO: delete
	m_dodec = SEntityManager::Instance().AddEntity("dodec");
	m_cube = SEntityManager::Instance().AddEntity("cube");
	
	SEntityManager::Instance().Update(); // initial update to get the new entity in the list

	SCamera::Instance().mode = SCamera::FIXEDTARGET;
	SCamera::Instance().SetTarget(m_player);
	SCamera::Instance().SetTargetDistance(m_camera_distance);

	SRenderer::Instance().SetFloorEntity(m_floor);
}

void MainScene::InstantiateGOs() {
	// -------------------------------------------------------------------
	// WALLS
	// -------------------------------------------------------------------
	float width = 50.0f;
	float depth = 40.0f;
	float height = 25.0f;
	float thickness = 5.0f;

	std::shared_ptr<Model> cube_model = SModelManager::Instance().LoadModel("data/cube.obj");

	CMesh floor_mesh = {};
	floor_mesh.model = cube_model;
	floor_mesh.colour = Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f));
	SEntityManager::Instance().AddComponent<CMesh>(m_floor, floor_mesh);

	CRigidBody floor_rb = {};
	floor_rb.velocity = Vector3(0, 0, 0);
	floor_rb.is_static = true;
	floor_rb.mass = 50.0f;
	SEntityManager::Instance().AddComponent<CRigidBody>(m_floor, floor_rb);

	CTransform floor_trans = {};
	floor_trans.rotation = Vector3(0, 0, 0);
	floor_trans.position = Vector3(0, -height, 0);
	floor_trans.scale = Vector3(width, thickness, depth);
	SEntityManager::Instance().AddComponent<CTransform>(m_floor, floor_trans); // floor

	CCollider floor_collider = {};
	floor_collider.center = floor_trans.position;
	floor_collider.volume_type = AABB;
	floor_collider.half_size = floor_trans.scale; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(m_floor, floor_collider);

	// -------------------------------------------------------------------
	// PLAYER
	// -------------------------------------------------------------------
	std::shared_ptr<Model> pyramid_model = SModelManager::Instance().LoadModel("data/pyramid.obj");

	CMesh player_mesh = {};
	player_mesh.model = pyramid_model;
	player_mesh.colour = Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f));
	SEntityManager::Instance().AddComponent<CMesh>(m_player, player_mesh);

	CTransform player_trans = {};
	player_trans.position = Vector3(0, 0, 0);
	player_trans.scale = Vector3(1.0f, 1.0f, 1.0f);
	SEntityManager::Instance().AddComponent<CTransform>(m_player, player_trans); // front face

	CRigidBody player_rb = {};
	player_rb.velocity = Vector3(0, 0, 0);
	player_rb.bounciness = 1.0f; // i wanna be bouncy :D
	player_rb.is_static = false;
	SEntityManager::Instance().AddComponent<CRigidBody>(m_player, player_rb);

	CCollider player_collider = {};
	player_collider.center = player_trans.position;
	player_collider.volume_type = SPHERE;
	player_collider.radius = (player_trans.scale.x / 2 + player_trans.scale.y / 2 + player_trans.scale.z / 2) / 3.0f; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(m_player, player_collider);

	// -------------------------------------------------------------------
	// DODEC & CUBE
	// -------------------------------------------------------------------
	std::shared_ptr<Model> dodec_model = SModelManager::Instance().LoadModel("data/dodecahedron.obj");

	CMesh dodec_mesh = {};
	dodec_mesh.model = dodec_model;
	dodec_mesh.colour = Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f));
	SEntityManager::Instance().AddComponent<CMesh>(m_dodec, dodec_mesh);

	CTransform dodec_trans = {};
	dodec_trans.rotation = Vector3(0, 0, 0);
	dodec_trans.position = Vector3(20.0f, 0, 0);
	dodec_trans.scale = Vector3(1.0f, 1.0f, 1.0f);
	SEntityManager::Instance().AddComponent<CTransform>(m_dodec, dodec_trans); // front face

	CRigidBody dodec_rb = {};
	dodec_rb.velocity = Vector3(0, 0, 0);
	dodec_rb.is_static = false;
	SEntityManager::Instance().AddComponent<CRigidBody>(m_dodec, dodec_rb);

	CCollider dodec_collider = {};
	dodec_collider.center = dodec_trans.position;
	dodec_collider.volume_type = SPHERE;
	dodec_collider.radius = (dodec_trans.scale.x + dodec_trans.scale.y + dodec_trans.scale.z) / 3.0f; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(m_dodec, dodec_collider);

	CMesh cube_mesh = {};
	cube_mesh.model = cube_model;
	cube_mesh.colour = Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f));
	SEntityManager::Instance().AddComponent<CMesh>(m_cube, cube_mesh);

	CTransform cube_trans = {};
	cube_trans.rotation = Vector3(0, 0, 0);
	cube_trans.position = Vector3(-20.0f, 0, 0);
	cube_trans.scale = Vector3(5.0f, 5.0f, 5.0f);
	SEntityManager::Instance().AddComponent<CTransform>(m_cube, cube_trans); // front face

	CRigidBody cube_rb = {};
	cube_rb.velocity = Vector3(0, 0, 0);
	cube_rb.is_static = false;
	SEntityManager::Instance().AddComponent<CRigidBody>(m_cube, cube_rb);

	CCollider cube_collider = {};
	cube_collider.center = cube_trans.position;
	cube_collider.volume_type = AABB;
	cube_collider.half_size = cube_trans.scale; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(m_cube, cube_collider);

	// -------------------------------------------------------------------
	// BOIDS
	// -------------------------------------------------------------------
	std::shared_ptr<Model> tetra_model = SModelManager::Instance().LoadModel("data/tetrahedron.obj");

	CMesh boid_mesh = {};
	boid_mesh.model = tetra_model;
	boid_mesh.colour = Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f));
	SEntityManager::Instance().AddComponent<CMesh>(m_boid, boid_mesh);

	CTransform boid_trans = {};
	boid_trans.rotation = Vector3(0, 0, (float)M_PI / 3.0f);
	boid_trans.position = Vector3(0, 0, 0);
	boid_trans.scale = Vector3(1.0f, 1.0f, 1.0f);
	SEntityManager::Instance().AddComponent<CTransform>(m_boid, boid_trans); // front face

	CRigidBody boid_rb = {};
	boid_rb.velocity = Vector3(0, 0, 0);
	boid_rb.is_static = true;
	SEntityManager::Instance().AddComponent<CRigidBody>(m_boid, boid_rb);

	CCollider boid_collider = {};
	boid_collider.center = boid_trans.position;
	boid_collider.volume_type = SPHERE;
	boid_collider.radius = (boid_trans.scale.x / 2 + boid_trans.scale.y / 2 + boid_trans.scale.z / 2) / 3.0f; // since I know the cube model is a unit cube
	SEntityManager::Instance().AddComponent<CCollider>(m_boid, boid_collider);

	CBoid cboid = {};
	cboid.forward = Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f));
	SEntityManager::Instance().AddComponent<CBoid>(m_boid, cboid);

	App::PlaySound("data/water-layer-1.wav", true); // loop

	m_sinput.Init(m_player, player_rb.mass); // init the input handler
}

void MainScene::Update(const float deltaTime) {
	m_sinput.Update(deltaTime);
}

void MainScene::LateUpdate(const float deltaTime) {

}

void MainScene::Shutdown() {

}