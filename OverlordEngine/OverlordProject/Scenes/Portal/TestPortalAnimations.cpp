#include "stdafx.h"
#include "TestPortalAnimations.h"

#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Portal/PhongMaterial_Skinned.h"
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


	const auto pSkinnedMaterialFrame = MaterialManager::Get()->CreateMaterial<PhongMaterial_Skinned>();
	pSkinnedMaterialFrame->SetDiffuseTexture(L"Textures/Player/BallBot/ballbot_frame.dds");
	pSkinnedMaterialFrame->SetNormalTexture(L"Textures/Player/BallBot/ballbot_frame_normal.dds");

	const auto pSkinnedMaterialShell = MaterialManager::Get()->CreateMaterial<PhongMaterial_Skinned>();
	pSkinnedMaterialShell->SetDiffuseTexture(L"Textures/Player/BallBot/ballbot_shell.dds");
	pSkinnedMaterialShell->SetNormalTexture(L"Textures/Player/BallBot/ballbot_shell_normal.dds");

	const auto pSkinnedMaterialEye = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterialEye->SetDiffuseTexture(L"Textures/Player/BallBot/bot_eye_ring_lights.dds");

	const auto pObject = AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Player/BallBot.ovm"));


	pModel->SetMaterial(pSkinnedMaterialShell, 0);
	pModel->SetMaterial(pSkinnedMaterialFrame, 1);
	pModel->SetMaterial(pSkinnedMaterialEye, 2);

	pObject->GetTransform()->Scale(0.15f);

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
}