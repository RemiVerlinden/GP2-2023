#include "stdafx.h"
#include "PhongMaterial_Skinned.h"

PhongMaterial_Skinned::PhongMaterial_Skinned()
	:Material(L"Effects/Portal/PhongMaterial_Skinned.fx")
{
}

void PhongMaterial_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	SetVariable_Scalar(L"gUseTextureDiffuse", true);

	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture->GetShaderResourceView());
}

void PhongMaterial_Skinned::SetNormalTexture(const std::wstring& assetFile)
{
	SetVariable_Scalar(L"gUseTextureNormal", true);

	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gNormalMap", m_pNormalTexture->GetShaderResourceView());
}

void PhongMaterial_Skinned::SetSpecularTexture(const std::wstring& assetFile)
{

	m_pSpecularTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureSpecularIntensity", m_pSpecularTexture->GetShaderResourceView());
}

void PhongMaterial_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModelComponent) const
{
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	// set worldInverseTranspose for phong
	{
		// Assuming you have your world matrix

		// Load it into an XMMATRIX
		DirectX::XMMATRIX worldMatrixXMM = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());

		// Calculate the inverse
		DirectX::XMMATRIX inverseWorldMatrix;

		inverseWorldMatrix = DirectX::XMMatrixInverse(nullptr, worldMatrixXMM);


		// Transpose the inverse matrix
		DirectX::XMMATRIX worldInverseTranspose = DirectX::XMMatrixTranspose(inverseWorldMatrix);

		// If you need to pass it back to a XMFLOAT4X4 for passing to the shader
		DirectX::XMFLOAT4X4 worldInverseTransposeFloat4x4;
		DirectX::XMStoreFloat4x4(&worldInverseTransposeFloat4x4, worldInverseTranspose);

		SetVariable_Matrix(L"gWorldInverseTranspose", worldInverseTransposeFloat4x4);
	}

	// set boneTransforms for skinned phong
	{
		//Retrieve The Animator from the ModelComponent
		ModelAnimator* pAnimator = pModelComponent->GetAnimator();

		//Make sure the animator is not NULL (ASSERT_NULL_)
		ASSERT_NULL_(pAnimator);

		//Retrieve the BoneTransforms from the Animator
		const std::vector<XMFLOAT4X4>& boneTransforms = pAnimator->GetBoneTransforms();

		//Set the 'gBones' variable of the effect (MatrixArray) > BoneTransforms
		SetVariable_MatrixArray(L"gBones", (float*)boneTransforms.data(), (UINT)boneTransforms.size());
	}

}
