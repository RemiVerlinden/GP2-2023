//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "TestScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/TorusPrefab.h"

TestScene::TestScene() :
	GameScene(L"TestScene") {}

void TestScene::Initialize()
{
	//m_SceneContext.settings.showInfoOverlay = true;
	//m_SceneContext.settings.drawPhysXDebug = true;
	//m_SceneContext.settings.drawGrid = true;
	m_SceneContext.settings.enableOnGUI = true;

	Logger::LogInfo(L"Welcome, you are now in the Test Scene!");

	m_pTorusBig = new TorusPrefab(10.f, 50, 1.5, 25, XMFLOAT4{Colors::Red});
	m_pTorusMiddle = new TorusPrefab(7.f, 50, 1.5, 25, XMFLOAT4{Colors::Green});
	m_pTorusSmall = new TorusPrefab(4.f, 50, 1.5, 25, XMFLOAT4{Colors::Blue});

	AddChild(m_pTorusBig)->AddChild(m_pTorusMiddle)->AddChild(m_pTorusSmall);
}

void TestScene::Update()
{
	float rotation = 90.f * m_SceneContext.pGameTime->GetTotal();
	m_pTorusBig->GetTransform()->Rotate(0, rotation,0);
	m_pTorusMiddle->GetTransform()->Rotate(rotation, 0,0);
	m_pTorusSmall->GetTransform()->Rotate(0, rotation, 0);
}

void TestScene::Draw()
{
	//Optional
}

void TestScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}
