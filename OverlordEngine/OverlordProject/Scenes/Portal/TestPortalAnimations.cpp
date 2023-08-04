#include "stdafx.h"
#include "TestPortalAnimations.h"

#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Portal/PhongMaterial_Skinned.h"
#include "Materials/Portal/Glass/GlassMaterial_Skinned.h"
#include "Materials/Portal/Weapon/PortalGun_Glow_Material.h"
#include "Materials/ColorMaterial.h"
  
TestPortalAnimations::~TestPortalAnimations()
{
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		delete[] m_ClipNames[i];
	}

	delete[] m_ClipNames;
}

void TestPortalAnimations::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	const XMFLOAT4 aliceBlue{ DirectX::Colors::Black };
	m_SceneContext.settings.clearColor = aliceBlue;


	//const auto pSkinnedMaterialFrame = MaterialManager::Get()->CreateMaterial<PhongMaterial_Skinned>();
	//pSkinnedMaterialFrame->SetDiffuseTexture(L"Textures/Player/BallBot/ballbot_frame.dds");
	//pSkinnedMaterialFrame->SetNormalTexture(L"Textures/Player/BallBot/ballbot_frame_normal.dds");

	//const auto pSkinnedMaterialShell = MaterialManager::Get()->CreateMaterial<PhongMaterial_Skinned>();
	//pSkinnedMaterialShell->SetDiffuseTexture(L"Textures/Player/BallBot/ballbot_shell.dds");
	//pSkinnedMaterialShell->SetNormalTexture(L"Textures/Player/BallBot/ballbot_shell_normal.dds");

	//const auto pSkinnedMaterialEye = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	//pSkinnedMaterialEye->SetDiffuseTexture(L"Textures/Player/BallBot/bot_eye_ring_lights.dds");

	//const auto pObject = AddChild(new GameObject);
	//const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Player/BallBot.ovm"));


		const auto pSkinnedMaterialGun = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
		pSkinnedMaterialGun->SetDiffuseTexture(L"Textures/Weapons/PortalgunRTX/Portalgun.dds");

		m_pPortalgunGlowMaterial = MaterialManager::Get()->CreateMaterial<PortalGun_Glow_Material>();
		m_PortalColor[0] = 0.f;
		m_PortalColor[1] = 0.6f;
		m_PortalColor[2] = 1.f;
		m_pPortalgunGlowMaterial->SetColor({ m_PortalColor[0],m_PortalColor[1], m_PortalColor[2] });

		const auto pSkinnedMaterialGlass = MaterialManager::Get()->CreateMaterial<GlassMaterial_Skinned>();
		pSkinnedMaterialGlass->SetDiffuseTexture(L"Textures/Weapons/PortalgunRTX/Glass.dds");
		pSkinnedMaterialGlass->SetOpacity(1.f);

	const auto pObject = AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Weapon/PortalgunRTX.ovm"));


	pModel->SetMaterial(pSkinnedMaterialGun, 0);
	pModel->SetMaterial(m_pPortalgunGlowMaterial, 1);
	pModel->SetMaterial(pSkinnedMaterialGlass, 2);



	pAnimator = pModel->GetAnimator();
	pAnimator->SetAnimation(m_AnimationClipId);
	pAnimator->SetAnimationSpeed(m_AnimationSpeed);

	//Gather Clip Names
	m_ClipCount = pAnimator->GetClipCount();
	m_ClipNames = new char* [m_ClipCount];
	for (UINT i{ 0 }; i < m_ClipCount; ++i)
	{
		auto clipName = StringUtil::utf8_encode(pAnimator->GetClip(static_cast<int>(i)).name);
		const auto clipSize = clipName.size();
		m_ClipNames[i] = new char[clipSize + 1];
		strncpy_s(m_ClipNames[i], clipSize + 1, clipName.c_str(), clipSize);
	}
}

void TestPortalAnimations::OnGUI()
{
	if (ImGui::Button(pAnimator->IsPlaying() ? "PAUSE" : "PLAY"))
	{
		if (pAnimator->IsPlaying())pAnimator->Pause();
		else pAnimator->Play();
	}

	if (ImGui::Button("RESET"))
	{
		pAnimator->Reset();
	}

	ImGui::Dummy({ 0,5 });

	bool reversed = pAnimator->IsReversed();
	if (ImGui::Checkbox("Play Reversed", &reversed))
	{
		pAnimator->SetPlayReversed(reversed);
	}

	if (ImGui::ListBox("Animation Clip", &m_AnimationClipId, m_ClipNames, static_cast<int>(m_ClipCount)))
	{
		pAnimator->SetAnimation(m_AnimationClipId);
	}

	if (ImGui::SliderFloat("Animation Speed", &m_AnimationSpeed, 0.f, 4.f))
	{
		pAnimator->SetAnimationSpeed(m_AnimationSpeed);
	}

	// Assuming m_PortalColor is a member variable of this class
	if (ImGui::ColorEdit3("Portal Glow Color", m_PortalColor))
	{
		m_pPortalgunGlowMaterial->SetColor({ m_PortalColor[0], m_PortalColor[1], m_PortalColor[2] });
	}

	// Assuming m_PortalColor is a member variable of this class
	ImGui::Text("Portal Glow Color: %.10f", pAnimator->GetBlendFactor());

}