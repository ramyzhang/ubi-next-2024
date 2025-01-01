#include "stdafx.h"

#include "MainScene.h"

void MainScene::InitGOs() {
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