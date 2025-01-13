#include "stdafx.h"

#include "MainScene.h"
#include "math/Utils.h"

void MainScene::InitGOs() {
	for (int i = 0; i < 10; i++) {
		SEntityManager::Instance().AddEntity("cube");
	}
	
	SEntityManager::Instance().Update(); // initial update to get the new entity in the list
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

		CTransform cube_transform = {};
		cube_transform.position = Vector3(0.0f, 0.0f, pos);
		cube_transform.rotation = Vector3(0, 0, 0);
		cube_transform.scale = Vector3(1, 1, 1);
		cube_transform.velocity = Vector3(0, 0, 0);
		SEntityManager::Instance().AddComponent<CTransform>(e_id, cube_transform);

		pos += 3.0f;
	}
}

void MainScene::Update(const float deltaTime) {
}

void MainScene::LateUpdate(const float deltaTime) {

}

void MainScene::Shutdown() {

}