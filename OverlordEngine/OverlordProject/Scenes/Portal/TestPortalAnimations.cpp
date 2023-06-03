#include "stdafx.h"
#include "TestPortalAnimations.h"

#include "Materials/DiffuseMaterial_Skinned.h"
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


	const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterial->SetDiffuseTexture(L"Textures/Maps/chamber02_static/materials_models_props_door_01.dds");

	const auto pObject = AddChild(new GameObject);
	const auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/DynamicProps/door.ovm"));
	pModel->SetMaterial(pSkinnedMaterial);

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