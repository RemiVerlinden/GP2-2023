#include "stdafx.h"
#include "ShadowMappingScene.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"


void ShadowMappingScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	//Materials
	//*********
	const auto pPeasantMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pPeasantMaterial->SetDiffuseTexture(L"Labs/Week8/Textures/PeasantGirl_Diffuse.png");

	const auto pPeasantMaterial2 = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pPeasantMaterial2->SetDiffuseTexture(L"Labs/Week8/Textures/PeasantGirl_Diffuse.png");

	const auto pGroundMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pGroundMaterial->SetDiffuseTexture(L"Labs/Week8/Textures/GroundBrick.jpg");

	//Ground Mesh
	//***********
	const auto pGroundObj = new GameObject();
	const auto pGroundModel = new ModelComponent(L"Labs/Week8/Meshes/UnitPlane.ovm");
	pGroundModel->SetMaterial(pGroundMaterial);

	pGroundObj->AddComponent(pGroundModel);
	pGroundObj->GetTransform()->Scale(10.0f, 10.0f, 10.0f);

	AddChild(pGroundObj);

	//Character Mesh
	//**************
	const auto pObject = AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Labs/Week8/Meshes/PeasantGirl.ovm"));
	pModel->SetMaterial(pPeasantMaterial);

	const auto pObject2 = AddChild(new GameObject());
	const auto pModel2 = pObject2->AddComponent(new ModelComponent(L"Labs/Week8/Meshes/PeasantGirl.ovm"));
	pModel2->SetMaterial(pPeasantMaterial2);

	pObject->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	pObject->GetTransform()->Translate(-20.f, 0.f, -20.f);
	pObject2->GetTransform()->Scale(0.5f, 0.1f, 0.5f);

	m_pSecondCharacter = pObject2;

	if (const auto pAnimator = pModel->GetAnimator())
	{
		pAnimator->SetAnimation(2);
		pAnimator->Play();
	}

	if (const auto pAnimator = pModel2->GetAnimator())
	{
		pAnimator->SetAnimation(0);
		pAnimator->Play();
	}

	//Input
	//*****
	m_SceneContext.pInput->AddInputAction(InputAction(0, InputState::pressed, VK_SPACE));
}

void ShadowMappingScene::Update()
{
	//Change Light Direction (SPACE > Camera Position/Direction)
	if (m_SceneContext.pInput->IsActionTriggered(0))
	{
		const auto pCameraTransform = m_SceneContext.pCamera->GetTransform();
		m_SceneContext.pLights->SetDirectionalLight(pCameraTransform->GetPosition(), pCameraTransform->GetForward());
	}
}

void ShadowMappingScene::PostDraw()
{
	//Draw ShadowMap (Debug Visualization)
	if (m_DrawShadowMap) {

		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { m_ShadowMapScale, m_ShadowMapScale }, { 1.f,0.f });
	}
}

void ShadowMappingScene::OnGUI()
{
	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);
	ImGui::SliderFloat("ShadowMap Scale", &m_ShadowMapScale, 0.f, 1.f);

	static XMFLOAT3 pos = { 20.f, 0.f, 20.f };
	static XMFLOAT3 scale = { 0.1f, 0.1f, 0.1f };
	ImGui::SliderFloat("x", &pos.x, -50.f, 50.f);
	ImGui::SliderFloat("y", &pos.z, -50.f, 50.f);
	ImGui::DragFloat3("scale", reinterpret_cast<float*>(&scale), 0.1f,0, 5);

	m_pSecondCharacter->GetTransform()->Translate(pos);
	m_pSecondCharacter->GetTransform()->Scale(scale);

	//MaterialManager::Get()->GetMaterial(2)->DrawImGui();
}