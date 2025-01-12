#include "stdafx.h"

#include "MainScene.h"

void MainScene::InitGOs() {
	SEntityManager::Instance().AddEntity("cube");

	SEntityManager::Instance().Update(); // initial update to get the new entity in the list

	EntityID cube_id = *SEntityManager::Instance().GetEntities("cube").begin();

	std::shared_ptr<Model> cube_model = SModelManager::Instance().LoadModel("data/cube.obj");

	CMesh cube_mesh = {};
	cube_mesh.model = cube_model;
	cube_mesh.colour = Vector3(0.5, 0.5, 0.5);
	SEntityManager::Instance().AddComponent<CMesh>(cube_id, cube_mesh);

	CTransform cube_transform = {};
	cube_transform.position = Vector3(0, 0, 0);
	cube_transform.rotation = Vector3(0, 0, 0);
	cube_transform.scale = Vector3(1, 1, 1);
	cube_transform.velocity = Vector3(0, 0, 0);
	SEntityManager::Instance().AddComponent<CTransform>(cube_id, cube_transform);
	
	SCamera::Instance().Init();
}

void MainScene::InstantiateGOs() {
}

void MainScene::Update(const float deltaTime) {
	SCamera::Instance().Update(deltaTime);
}

void MainScene::LateUpdate(const float deltaTime) {

}

void MainScene::Shutdown() {

}