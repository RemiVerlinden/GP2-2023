//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "UberMaterialScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"


UberMaterialScene::UberMaterialScene() :
	GameScene(L"UberMaterialScene"){}

void UberMaterialScene::Initialize()
{
	//m_SceneContext.settings.showInfoOverlay = true;
	//m_SceneContext.settings.drawPhysXDebug = true;
	//m_SceneContext.settings.drawGrid = true;
	m_SceneContext.settings.enableOnGUI = true;

	Logger::LogInfo(L"Welcome, humble Minion!");
}

void UberMaterialScene::Update()
{
	//Optional
}

void UberMaterialScene::Draw()
{
	//Optional
}

void UberMaterialScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}
