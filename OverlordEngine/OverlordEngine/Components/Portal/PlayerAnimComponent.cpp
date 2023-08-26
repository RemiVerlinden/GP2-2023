#include "stdafx.h"
#include "PlayerAnimComponent.h"
#include "../OverlordProject/Materials/Portal/PhongMaterial_Skinned.h"
#include "../OverlordProject/Materials/DiffuseMaterial_Skinned.h"
#include "../OverlordProject/Prefabs/Character.h"


PlayerAnimComponent::PlayerAnimComponent(Character* character)
{
	m_pCharacter = character;
}

void PlayerAnimComponent::SetAnimation(AnimationState state)
{
	if (m_AnimationState != state)
	{
		m_AnimationState = state;
		m_pAnimator->SetAnimation(state);
	}
}

void PlayerAnimComponent::Initialize(const SceneContext&)
{
	CreatePlayerMesh();

}

void PlayerAnimComponent::Update(const SceneContext& /*context*/)
{
	float yaw = m_pCharacter->GetYaw();
	GetTransform()->Rotate(0, yaw + 180,0);
}

void PlayerAnimComponent::CreatePlayerMesh()
{
	const auto pSkinnedMaterialFrame = MaterialManager::Get()->CreateMaterial<PhongMaterial_Skinned>();
	pSkinnedMaterialFrame->SetDiffuseTexture(L"Textures/Player/BallBot/ballbot_frame.dds");
	pSkinnedMaterialFrame->SetNormalTexture(L"Textures/Player/BallBot/ballbot_frame_normal.dds");

	const auto pSkinnedMaterialShell = MaterialManager::Get()->CreateMaterial<PhongMaterial_Skinned>();
	pSkinnedMaterialShell->SetDiffuseTexture(L"Textures/Player/BallBot/ballbot_shell.dds");
	pSkinnedMaterialShell->SetNormalTexture(L"Textures/Player/BallBot/ballbot_shell_normal.dds");

	const auto pSkinnedMaterialEye = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterialEye->SetDiffuseTexture(L"Textures/Player/BallBot/bot_eye_ring_lights.dds");

	const auto pObject = m_pGameObject;
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Player/BallBot.ovm"));
	
	pModel->SetPortalrRenderContext(ModelComponent::PortalRenderContext::PortalViewOnly);

	pModel->SetMaterial(pSkinnedMaterialShell, 0);
	pModel->SetMaterial(pSkinnedMaterialFrame, 1);
	pModel->SetMaterial(pSkinnedMaterialEye, 2);

	pObject->GetTransform()->Scale(0.04f); // sorry, if I had more time I4d make the model 1/1 scale

	
	m_pAnimator = pModel->GetAnimator();
	m_pAnimator->SetUseBlending(true);
	m_pAnimator->SetAnimation(0);
	m_pAnimator->Play();
	m_pAnimator->SetAnimationSpeed(1.f);
}




