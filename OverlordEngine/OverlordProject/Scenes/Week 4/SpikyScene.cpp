#include "stdafx.h"
#include "SpikyScene.h"
#include "Materials/SpikyMaterial.h"

void SpikyScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	// Create and add sphere GameObject
	m_pSphere = new GameObject();
	AddChild(m_pSphere);

	// Create SpikyMaterial instance
	m_pSpikyMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	// Add ModelComponent and set material
	ModelComponent* pModelComponent = m_pSphere->AddComponent(new ModelComponent(L"Labs/Week4/Meshes/OctaSphere.ovm"));
	pModelComponent->SetMaterial(m_pSpikyMaterial);

	// Scale the sphere
	m_pSphere->GetTransform()->Scale(12.f, 12.f, 12.f);
}

// Update the scene
void SpikyScene::Update()
{
	m_pSphere->GetTransform()->Rotate(0, m_SceneContext.pGameTime->GetTotal() * 20, 0);
}

// Draw the scene (Optional)
void SpikyScene::Draw()
{
}

// Handle ImGUI for the material
void SpikyScene::OnGUI()
{
	m_pSpikyMaterial->DrawImGui();
}

