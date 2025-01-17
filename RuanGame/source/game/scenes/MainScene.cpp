#include "stdafx.h"

#include "MainScene.h"

void MainScene::InitGOs() {
	m_player = SEntityManager::Instance().AddEntity("cube");
	SEntityManager::Instance().AddEntity("cube");
	SEntityManager::Instance().AddEntity("cube");
	
	SEntityManager::Instance().Update(); // initial update to get the new entity in the list

	SCamera::Instance().mode = SCamera::FIXEDTARGET;
	SCamera::Instance().SetTarget(m_player);
	SCamera::Instance().SetTargetDistance(m_camera_distance);

	m_sinput.Init(m_player);
}

void MainScene::InstantiateGOs() {
	EntityIDSet cube_ids = SEntityManager::Instance().GetEntities("cube");

	float pos = 0.0f;
	for (auto& e_id : cube_ids) {
		std::shared_ptr<Model> cube_model = SModelManager::Instance().LoadModel("data/cube.obj");

		CMesh cube_mesh = {};
		cube_mesh.model = cube_model;
		cube_mesh.colour = Vector3(RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f), RandomFloat(0.0f, 1.0f));
		SEntityManager::Instance().AddComponent<CMesh>(e_id, cube_mesh);

		float size = RandomFloat(1.0f, 2.0f);
		CTransform cube_transform = {};
		cube_transform.position = Vector3(pos, 0.0f, 0.0f);
		cube_transform.rotation = Vector3(0, 0, 0);
		cube_transform.scale = Vector3(size, size, size);
		SEntityManager::Instance().AddComponent<CTransform>(e_id, cube_transform);

		CCollider cube_collider = {};
		cube_collider.center = Vector3(pos, 0.0f, 0.0f);
		cube_collider.volume_type = AABB;
		cube_collider.half_size = Vector3(size, size, size); // since I know the cube model is a unit cube
		SEntityManager::Instance().AddComponent<CCollider>(e_id, cube_collider);

		CRigidBody cube_rb = {};
		cube_rb.velocity = Vector3(0, 0, 0);
		SEntityManager::Instance().AddComponent<CRigidBody>(e_id, cube_rb);

		pos += 10.0f;
	}
}

void MainScene::Update(const float deltaTime) {
	m_sinput.HandleInput(deltaTime);
}

void MainScene::LateUpdate(const float deltaTime) {

}

void MainScene::Shutdown() {

}