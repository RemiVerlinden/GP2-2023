#include "stdafx.h"
#include "PortalProps.h"

#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/ColorMaterial.h"
#include "Materials/Portal/PhongMaterial.h"


void PortalProps::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	m_pPhong = MaterialManager::Get()->CreateMaterial<PhongMaterial>();
	m_pPhong->SetDiffuseTexture(L"Textures/Maps/chamber02_static/materials_models_props_button.dds");
	m_pPhong->SetNormalTexture(L"Textures/Maps/chamber02_static/materials_models_props_button.dds");

	const auto pButtonTop = AddChild(new GameObject);
	pModelTop = pButtonTop->AddComponent(new ModelComponent(L"Meshes/DynamicProps/button_top.ovm"));
	pModelTop->SetMaterial(m_pPhong);

	m_pButtonAnim = pButtonTop->AddComponent(new ButtonAnimComponent(pButtonTop));

	pButtonTop->GetTransform()->Scale(0.15f);

	const auto pButtonBase = AddChild(new GameObject);
	const auto pModelbase = pButtonBase->AddComponent(new ModelComponent(L"Meshes/DynamicProps/button_base.ovm"));
	pModelbase->SetMaterial(m_pPhong);
	pButtonBase->GetTransform()->Scale(0.15f);

}

void PortalProps::OnGUI()
{
}

void PortalProps::Update()
{
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'N'))
	{
		static bool pressed = false;
		pressed = !pressed;
		m_pButtonAnim->SetPressed(pressed);
	}


	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'K'))
	{
		const auto pCameraTransform = m_SceneContext.pCamera->GetTransform();
		m_SceneContext.pLights->SetDirectionalLight(pCameraTransform->GetPosition(), pCameraTransform->GetForward());
	}
}