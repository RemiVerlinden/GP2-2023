#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Materials/UberMaterial.h"

// Constructor
UberMaterialScene::UberMaterialScene() :
    GameScene(L"UberMaterialScene")
{
}

// Initialize the scene
void UberMaterialScene::Initialize()
{
    m_SceneContext.settings.drawGrid = false;
    m_SceneContext.settings.enableOnGUI = true;

    // Create and add sphere GameObject
    m_pSphere = new GameObject{};
    AddChild(m_pSphere);

    // Create UberMaterial instance
    m_pUberMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();

    // Add ModelComponent and set material
    ModelComponent* pModelComponent = m_pSphere->AddComponent(new ModelComponent{ L"Labs/Week4/Meshes/Sphere.ovm" });
    pModelComponent->SetMaterial(m_pUberMaterial);

    // Scale the sphere
    m_pSphere->GetTransform()->Scale(20.0f, 20.0f, 20.0f);
}

// Update the scene
void UberMaterialScene::Update()
{
    m_pSphere->GetTransform()->Rotate(0, GetSceneContext().pGameTime->GetTotal() * 12.f, 0, true);
}

// Draw the scene (Optional)
void UberMaterialScene::Draw() {}

// Handle ImGUI for the material
void UberMaterialScene::OnGUI()
{
    m_pUberMaterial->DrawImGui();
}
