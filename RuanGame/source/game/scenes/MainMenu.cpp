#include "stdafx.h"

#include "math/Utils.h"
#include "MainMenu.h"

void MainMenu::InitGOs() {
	SCamera::Instance().mode = SCamera::CLAMPED;

	m_title = SEntityManager::Instance().AddEntity("ui");
	m_play = SEntityManager::Instance().AddEntity("ui");
	m_quit = SEntityManager::Instance().AddEntity("ui");
	m_indicator = SEntityManager::Instance().AddEntity("ui");

	m_border_top = SEntityManager::Instance().AddEntity("ui");
	m_border_bottom = SEntityManager::Instance().AddEntity("ui");

	for (int i = 0; i < m_length_vertical; i++) {
		m_border_left.push_back(SEntityManager::Instance().AddEntity("ui"));
		m_border_right.push_back(SEntityManager::Instance().AddEntity("ui"));
	}

	for (int i = 0; i < m_num_dodecs; i++) {
		m_dodecahedrons.push_back(SEntityManager::Instance().AddEntity("dodec"));
	}
	
	SEntityManager::Instance().Update(); // initial update to get the new entity in the list

	m_sbackground = SMenuBackground();
	m_sbackground.Init(m_dodecahedrons);

	m_sinput = SMenuInput();
	m_sinput.Init(m_indicator);
}

void MainMenu::InstantiateGOs() {
	// -------------------------------------------------------------------
	// UI ELEMENTS
	// -------------------------------------------------------------------
	Vector3 green = Vector3(0.3f, 0.8f, 0.44f);
	Vector3 yellow = Vector3(0.8f, 0.67f, 0.31f);

	CUIText ctitle = {};
	ctitle.pos = Vector3(500.0f, 480.0f, 0.0f);
	ctitle.rgb = green;
	ctitle.text = "Ruan: The Game";
	SEntityManager::Instance().AddComponent<CUIText>(m_title, ctitle);

	CUIText cplay = {};
	cplay.pos = Vector3(500.0f, 410.0f, 0.0f);
	cplay.rgb = green;
	cplay.text = "play";
	SEntityManager::Instance().AddComponent<CUIText>(m_play, cplay);
	
	CUIText cquit = {};
	cquit.pos = Vector3(500.0f, 380.0f, 0.0f);
	cquit.rgb = green;
	cquit.text = "quit";
	SEntityManager::Instance().AddComponent<CUIText>(m_quit, cquit);
	
	CUIText cindic = {};
	cindic.pos = Vector3(450.0f, 410.0f, 0.0f);
	cindic.rgb = yellow;
	cindic.text = ">";
	SEntityManager::Instance().AddComponent<CUIText>(m_indicator, cindic);

	CUIText cborder_horizontal = {};
	cborder_horizontal.rgb = green;
	for (int i = 0; i < m_length_horizontal; i++) {
		cborder_horizontal.text.append("/");
	}
	cborder_horizontal.pos = Vector3(12.0f, 748.0f, 0.0f);
	SEntityManager::Instance().AddComponent<CUIText>(m_border_top, cborder_horizontal);
	cborder_horizontal.pos = Vector3(12.0f, 7.0f, 0.0f);
	SEntityManager::Instance().AddComponent<CUIText>(m_border_bottom, cborder_horizontal);

	for (int i = 0; i < m_length_vertical; i++) {
		CUIText cborder_vertical = {};
		cborder_vertical.rgb = green;
		cborder_vertical.text = "/";

		float gap = static_cast<float>(i) * 13;
		cborder_vertical.pos = Vector3(12.0f, 748.0f - gap, 0.0f);
		SEntityManager::Instance().AddComponent<CUIText>(m_border_left[i], cborder_vertical);
		cborder_vertical.pos = Vector3(1004.0f, 748.0f - gap, 0.0f);
		SEntityManager::Instance().AddComponent<CUIText>(m_border_right[i], cborder_vertical);
	}

	// -------------------------------------------------------------------
	// BACKGROUND
	// -------------------------------------------------------------------
	for (int i = 0; i < m_num_dodecs; i++) {
		std::shared_ptr<Model> dodec_model = SModelManager::Instance().LoadModel("data/dodecahedron.obj");

		CMesh cmesh = {};
		cmesh.model = dodec_model;
		cmesh.colour = Vector3(RandomFloat(0.0f, 0.2f), RandomFloat(0.65f, 0.8f), RandomFloat(0.55f, 0.75f));
		SEntityManager::Instance().AddComponent<CMesh>(m_dodecahedrons[i], cmesh);

		float size = RandomFloat(5.0f, 10.0f);
		CTransform ctrans = {};
		ctrans.position = Vector3(RandomFloat(-20.0f, 20.0f), RandomFloat(-20.0f, 20.0f), RandomFloat(-10.0f, 100.0f));
		ctrans.rotation = Vector3(0, 0, 0);
		ctrans.scale = Vector3(size, size, size);
		SEntityManager::Instance().AddComponent<CTransform>(m_dodecahedrons[i], ctrans);
	}

	SCamera::Instance().position = Vector3(50.0f, 0.0f, 50.0f);
	SCamera::Instance().yaw = -1.33f;

	m_pos_zoomed = SCamera::Instance().position;

	App::PlaySound("data/mainmenu.wav", true); // loop
}

void MainMenu::Update(const float deltaTime) {
	// if "PLAY" was pressed, go to the next scene
	MenuResults res = m_sinput.HandleInput(deltaTime);

	if (res == PLAY) SetState(SCENE_NEXT);
	else if (res == QUIT) glutLeaveMainLoop();

	// update the background still, before the next frame
	m_sbackground.Update(deltaTime);
}

void MainMenu::LateUpdate(const float deltaTime) {
	// if the indicator is on the play button, "zoom" in
	if (m_sinput.indic_play) {
		Vector3 orig_pos = SCamera::Instance().position;

		if (orig_pos.distance_squared(m_pos_zoomed) < 0.5f) return;

		Vector3 new_pos = Lerp(orig_pos, m_pos_zoomed, 0.01f * deltaTime);
		SCamera::Instance().position = new_pos;
	}
	// if the indicator is on the quit button, "zoom" out
	else {
		Vector3 orig_pos = SCamera::Instance().position;
		Vector3 zoomed_out = m_pos_zoomed - SCamera::Instance().look * 20.0f;

		if (orig_pos.distance_squared(zoomed_out) < 0.5f) return;

		Vector3 new_pos = Lerp(orig_pos, zoomed_out, 0.01f * deltaTime);
		SCamera::Instance().position = new_pos;
	}
}

void MainMenu::Shutdown() {
	SEntityManager::Instance().ClearAllEntities(); //

	App::StopSound("data/water-menu.wav");
}
