#include "stdafx.h"
#include "GlassMaterial_Skinned.h"

GlassMaterial_Skinned::GlassMaterial_Skinned() :
	Material(L"Effects/Portal/GlassMaterial_Skinned.fx")
{
}

void GlassMaterial_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture->GetShaderResourceView());
}

void GlassMaterial_Skinned::SetOpacity(const float opacity)
{
	m_Opacity = opacity;
	SetVariable_Scalar(L"gOpacity", m_Opacity);
}

void GlassMaterial_Skinned::InitializeEffectVariables()
{
	m_EnableShadows = true;
}

void GlassMaterial_Skinned::OnUpdateModelVariables(const SceneContext&, const ModelComponent* pModel) const
{
	//Retrieve The Animator from the ModelComponent
	ModelAnimator* pAnimator = pModel->GetAnimator();

	//Make sure the animator is not NULL (ASSERT_NULL_)
	ASSERT_NULL_(pAnimator);

	//Retrieve the BoneTransforms from the Animator
	const std::vector<XMFLOAT4X4>& boneTransforms = pAnimator->GetBoneTransforms();

	//Set the 'gBones' variable of the effect (MatrixArray) > BoneTransforms
	SetVariable_MatrixArray(L"gBones", (float*)boneTransforms.data(), (UINT)boneTransforms.size());

		// I want to disable shadows on this material for now until end of exam.
	SetVariable_Scalar(L"gEnableShadows", false);
}
