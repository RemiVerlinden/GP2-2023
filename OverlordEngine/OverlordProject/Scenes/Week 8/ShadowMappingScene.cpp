#include "stdafx.h"
#include "ShadowMappingScene.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Materials/Portal/SkyBoxMaterial.h"
#include "Materials\ColorMaterial.h"
#include "../OverlordProject/Prefabs/ArrowObject.h"


void ShadowMappingScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_SceneContext.pLights->SetDirectionalLight({ -95.6139526f,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });
	Light spotlight;
	spotlight.direction = { 0.740129888f, -0.597205281f, 0.309117377f, 1.f };
	spotlight.position = { -95.6139526f,66.1346436f,-41.1850471f, 11.f };
	spotlight.spotLightAngle = m_SceneContext.pCamera->GetFieldOfView();
	spotlight.spotLightAngle = XMConvertToRadians(90.f);
	m_SceneContext.pLights->AddLight(spotlight);

	//auto MakeArrowObject = [&](const XMFLOAT4& color) -> GameObject*
	//{
	//	GameObject* pArrowObject = AddChild(new GameObject());
	//	auto pMeshComponent = pArrowObject->AddComponent(new ModelComponent(L"Meshes/Arrow.ovm"));
	//	auto pArrowMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	//	pArrowMaterial->SetColor(color);
	//	pMeshComponent->SetMaterial(pArrowMaterial);

	//	return pArrowObject;
	//};
	//MakeArrowObject(XMFLOAT4(Colors::White));

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

	// SKYBOX
	{
		m_pSkybox = new GameObject();

		const auto pSkyBoxMaterial = MaterialManager::Get()->CreateMaterial<SkyBoxmaterial>();
		pSkyBoxMaterial->SetSkyBoxTexture(L"Textures/earth-cubemap.dds");

		m_pSkybox->AddComponent(new ModelComponent(L"Meshes/Box.ovm"));
		m_pSkybox->GetComponent<ModelComponent>()->SetMaterial(pSkyBoxMaterial);
		AddChild(m_pSkybox);
	}

	// TESTING
	{
		const auto pMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
		pMaterial->SetDiffuseTexture(L"Labs/Week8/Textures/GroundBrick.jpg");

		m_pBoneObject = new ArrowObject(pMaterial);
		AddChild(m_pBoneObject);
		m_pBoneObject->GetTransform()->Translate(0, 50, 0);
		m_pBoneObject->GetTransform()->Scale(5,5,5);
		//{
		//	// Get the eyePosition and direction from the spotlight
		//	auto pos = XMFLOAT3{ 0, 0, 0 };
		//	auto eyePosition = XMLoadFloat3(&pos);

		//	XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);

		//	const XMFLOAT3 direction = XMFLOAT3(0.0f, 0.0f, 1.0f);

		//	XMMATRIX viewproj, view, projection;

		//	projection = XMMatrixPerspectiveFovLH(XM_PI / 2, 1, 0.1f, 200);
		//	view = XMMatrixLookAtLH(eyePosition, eyePosition + XMLoadFloat3(&direction), upVec);
		//	viewproj = view * projection;

		//	XMFLOAT4X4 finalMatrix;
		//	XMStoreFloat4x4(&finalMatrix, viewproj);
		//	m_pBoneObject->GetTransform()->SetTransform(finalMatrix);
		//}
	}
}

void ShadowMappingScene::Update()
{
	//Change Light Direction (SPACE > Camera Position/Direction)
	if (m_SceneContext.pInput->IsActionTriggered(0))
	{
		//m_SceneContext.pLights->SetDirectionalLight(pCameraTransform->GetPosition(), pCameraTransform->GetForward());

		const auto pCameraTransform = m_SceneContext.pCamera->GetTransform();
		XMFLOAT3 pos = pCameraTransform->GetPosition();
		XMFLOAT3 forward = pCameraTransform->GetForward();

		auto& spotlight = m_SceneContext.pLights->GetLight(0);
		spotlight.position = XMFLOAT4(pos.x, pos.y, pos.z, 0.0f);
		spotlight.direction = XMFLOAT4(forward.x, forward.y, forward.z, 0.0f);
		spotlight.up = pCameraTransform->GetUp();
	}
}

void ShadowMappingScene::PostDraw()
{
	//Draw ShadowMap (Debug Visualization)
	if (m_DrawShadowMap)
	{
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
	ImGui::DragFloat3("scale", reinterpret_cast<float*>(&scale), 0.1f, 0, 5);

	m_pSecondCharacter->GetTransform()->Translate(pos);
	m_pSecondCharacter->GetTransform()->Scale(scale);

	//MaterialManager::Get()->GetMaterial(2)->DrawImGui();
}