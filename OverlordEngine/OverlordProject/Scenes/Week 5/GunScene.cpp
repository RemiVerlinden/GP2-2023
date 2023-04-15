//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "GunScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

#include "Materials/ColorMaterial.h"

GunScene::GunScene() :
	GameScene(L"GunScene")
{
}

void GunScene::Initialize()
{
	InitializeGunObject();
}


void GunScene::InitializeGunObject()
{
	m_SceneContext.settings.enableOnGUI = true;

	AddModel(L"Meshes/Weapon/Portalgun.ovm");
	m_pModelsVec[0]->GetTransform()->Rotate(90,0,0);

	AddModel(L"Meshes/PeasantGirl.ovm");
}

void GunScene::AddModel(std::wstring assetfile)
{
	GameObject* go = new GameObject();
	AddChild(go);

	ModelComponent* model = new ModelComponent(assetfile);
	go->AddComponent(model);

	// Apply a diffuse material to the chair model
	ColorMaterial* pColorMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	model->SetMaterial(pColorMat);

	m_pModelsVec.push_back(go);
}


void GunScene::Update()
{
	//Optional
}

void GunScene::Draw()
{
	//Optional
}

void GunScene::OnGUI()
{
	static XMFLOAT3 rotation0{};
	static float scale0{1};
	static XMFLOAT3 rotation1{};
	static float scale1{0};
	ImGui::DragFloat3("Rot0", ConvertUtil::ToImFloatPtr(rotation0));
	m_pModelsVec[0]->GetTransform()->Rotate(rotation0);

	ImGui::DragFloat("Scale0", &scale0);
	m_pModelsVec[0]->GetTransform()->Scale(scale0);

	ImGui::DragFloat3("Rot1", ConvertUtil::ToImFloatPtr(rotation1));
	m_pModelsVec[1]->GetTransform()->Rotate(rotation1);

	ImGui::DragFloat("Scale1", &scale1);
	m_pModelsVec[1]->GetTransform()->Scale(scale1);

}

