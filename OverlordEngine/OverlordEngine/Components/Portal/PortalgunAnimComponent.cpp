#include "stdafx.h"
#include "PortalgunAnimComponent.h"
#include "../OverlordProject/Materials/Portal/PhongMaterial_Skinned.h"
#include "../OverlordProject/Materials/DiffuseMaterial_Skinned.h"
#include "../OverlordProject/Prefabs/Character.h"
#include "../OverlordProject/Materials/Portal/Glass/GlassMaterial_Skinned.h"
#include "../OverlordProject/Materials/Portal/Weapon/PortalGun_Glow_Material.h"
#include "../OverlordEngine/Components/Portal/PortalgunStatemachineComponent.h"

#include "../OverlordProject/Portal/ViewBob.h"

PortalgunAnimComponent::PortalgunAnimComponent(Character* character)
{
	m_pCharacter = character;
}

void PortalgunAnimComponent::SetAnimation(AnimationState state, bool playOnce)
{
	static AnimationState latestState = AnimationState::Idle; // Idle as default value

	if (latestState != state || !m_pAnimator->IsPlaying())
	{
		latestState = state;
		m_pAnimator->SetAnimation(state);

		if (playOnce) m_pAnimator->PlayOnce();
	}
}

bool PortalgunAnimComponent::IsAnimationFinished() const
{
	return !m_pAnimator->IsPlaying();
}

void PortalgunAnimComponent::SetPortalgunColor(const XMFLOAT3& color)
{
	m_pPortalgunGlowMaterial->SetColor(color);
}

void PortalgunAnimComponent::Initialize(const SceneContext&)
{
	m_pViewBob = std::make_unique<ViewBob>(m_pCharacter);

	CreatePortalgunMesh();
	m_pGameObject->AddComponent(new PortalgunStatemachineComponent(this));
}

void PortalgunAnimComponent::Update(const SceneContext& context)
{
	DirectX::XMFLOAT3 origin = { 0.0f, 0.0f, 0.0f }; // Replace with your camera's origin
	DirectX::XMFLOAT3 angles = { 0.0f, 0.0f, 0.0f }; // Replace with your camera's angles


	m_pViewBob->CalcViewmodelBob(context.pGameTime->GetTotal());
	m_pViewBob->AddViewmodelBob(origin, angles);

	origin.z += -0.2f;

	XMFLOAT3 originalAngle;

	originalAngle.y = -m_pCharacter->GetYaw();
	originalAngle.x = m_pCharacter->GetPitch();
	originalAngle.z = 0;

	XMFLOAT3 finalAngle = originalAngle;
	finalAngle.x += angles.x;
	finalAngle.y += angles.y;
	finalAngle.z += angles.z;

	angles.y -= 90.f;
	m_pViewBob->CalcViewModelLag(context.pGameTime->GetElapsed(), origin, finalAngle, originalAngle);

	finalAngle.x = MathUtil::DegToRad(angles.x);
	finalAngle.y = MathUtil::DegToRad(angles.y);
	finalAngle.z = MathUtil::DegToRad(angles.z);


	// Create a quaternion from the Euler angles
	DirectX::XMVECTOR deltaRotation = DirectX::XMQuaternionRotationRollPitchYaw(finalAngle.x, finalAngle.y, finalAngle.z);
	m_pPortalgunHolder->GetTransform()->Rotate(deltaRotation);
	m_pPortalgunHolder->GetTransform()->Translate(origin);

}


void PortalgunAnimComponent::CreatePortalgunMesh()
{
	const auto pSkinnedMaterialGun = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterialGun->SetDiffuseTexture(L"Textures/Weapons/PortalgunRTX/Portalgun.dds");

	m_pPortalgunGlowMaterial = MaterialManager::Get()->CreateMaterial<PortalGun_Glow_Material>();
	m_PortalColor[0] = 1.f;
	m_PortalColor[1] = 1.f;
	m_PortalColor[2] = 1.f;
	m_pPortalgunGlowMaterial->SetColor({ m_PortalColor[0],m_PortalColor[1], m_PortalColor[2] });

	const auto pSkinnedMaterialGlass = MaterialManager::Get()->CreateMaterial<GlassMaterial_Skinned>();
	pSkinnedMaterialGlass->SetDiffuseTexture(L"Textures/Weapons/PortalgunRTX/Glass.dds");
	pSkinnedMaterialGlass->SetOpacity(1.f);

	m_pPortalgunHolder = m_pGameObject->AddChild(new GameObject());
	const auto pObject = m_pPortalgunHolder;
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Weapon/PortalgunRTX.ovm"));

	pModel->SetPortalrRenderContext(ModelComponent::PortalRenderContext::RealWorldOnly);

	pObject->GetTransform()->Scale(0.04f); // sorry, if I had more time I'd make the model 1/1 scale
	pObject->GetTransform()->Rotate(0.f, -90.f, 0.f);
	pObject->GetTransform()->Translate(0.f, 0.f, -0.2f);


	pModel->SetMaterial(pSkinnedMaterialGun, 0);
	pModel->SetMaterial(m_pPortalgunGlowMaterial, 1);
	pModel->SetMaterial(pSkinnedMaterialGlass, 2);


	m_pAnimator = pModel->GetAnimator();
	m_pAnimator->SetUseBlending(true);
	m_pAnimator->SetAnimation(AnimationState::Idle); // idle as default value
	m_pAnimator->Play();
	m_pAnimator->SetAnimationSpeed(1.f);

}
void PortalgunAnimComponent::TogglePortalgunModelRender(bool shouldRender)
{
	auto pModel = m_pPortalgunHolder->GetComponent<ModelComponent>();
	ModelComponent::PortalRenderContext portalRenderContext = shouldRender ? ModelComponent::PortalRenderContext::RealWorldOnly : ModelComponent::PortalRenderContext::PortalViewOnly;
	pModel->SetPortalrRenderContext(portalRenderContext);
}


