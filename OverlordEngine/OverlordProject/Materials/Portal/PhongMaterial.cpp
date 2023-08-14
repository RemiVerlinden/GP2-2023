#include "stdafx.h"
#include "PhongMaterial.h"

PhongMaterial::PhongMaterial()
	:Material(L"Effects/Portal/PhongMaterial.fx")
{
}

void PhongMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	SetVariable_Scalar(L"gUseTextureDiffuse", true);

	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture->GetShaderResourceView());
}

void PhongMaterial::SetNormalTexture(const std::wstring& assetFile)
{
	SetVariable_Scalar(L"gUseTextureNormal", true);

	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gNormalMap", m_pNormalTexture->GetShaderResourceView());
}

void PhongMaterial::SetSpecularTexture(const std::wstring& assetFile)
{

	m_pSpecularTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureSpecularIntensity", m_pSpecularTexture->GetShaderResourceView());
}

void PhongMaterial::InitializeEffectVariables()
{
	m_EnableShadows = true;
}

void PhongMaterial::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModelComponent) const
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

}
