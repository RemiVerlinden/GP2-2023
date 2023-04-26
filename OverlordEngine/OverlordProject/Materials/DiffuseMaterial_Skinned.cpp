	#include "stdafx.h"
#include "DiffuseMaterial_Skinned.h"

DiffuseMaterial_Skinned::DiffuseMaterial_Skinned():
	Material(L"Effects/PosNormTex3D_Skinned.fx")
{
}

void DiffuseMaterial_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Skinned::OnUpdateModelVariables(const SceneContext&, const ModelComponent* pModel) const
{

	//Retrieve The Animator from the ModelComponent
	ModelAnimator* pAnimator = pModel->GetAnimator();

	//Make sure the animator is not NULL (ASSERT_NULL_)
	ASSERT_NULL_(pAnimator);

	//Retrieve the BoneTransforms from the Animator
	const std::vector<XMFLOAT4X4>& boneTransforms = pAnimator->GetBoneTransforms();

	//Set the 'gBones' variable of the effect (MatrixArray) > BoneTransforms
	SetVariable_MatrixArray(L"gBones", (float*)boneTransforms.data(), (UINT)boneTransforms.size());



}
