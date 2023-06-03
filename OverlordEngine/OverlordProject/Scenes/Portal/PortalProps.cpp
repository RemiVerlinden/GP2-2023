#include "stdafx.h"
#include "PortalProps.h"

#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/ColorMaterial.h"


void PortalProps::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	pSkinnedMaterial->SetColor({ 1,0,0,1 });

	const auto pObject = AddChild(new GameObject);
	pModel = pObject->AddComponent(new ModelComponent(L"Meshes/DynamicProps/button_top.ovm"));
	pModel->SetMaterial(pSkinnedMaterial);

	m_pButtonAnim = pObject->AddComponent(new ButtonAnimComponent(pObject));

	pObject->GetTransform()->Scale(0.15f);
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
}