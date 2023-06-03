#include "stdafx.h"
#include "PhongMaterial.h"

PhongMaterial::PhongMaterial()
	:Material(L"Effects/Portal/PhongMaterial2.fx")
{
}

void PhongMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	SetVariable_Scalar(L"gUseTextureDiffuse", true);


	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture->GetShaderResourceView());
}

void PhongMaterial::SetNormalTexture(const std::wstring& assetFile)
{
	SetVariable_Scalar(L"gUseTextureNormal", true);

	m_pNormalTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureNormal", m_pNormalTexture->GetShaderResourceView());
}

void PhongMaterial::SetSpecularTexture(const std::wstring& assetFile)
{
	m_UseSpecularTexture = true;
	SetVariable_Scalar(L"gUseSpecular", m_UseSpecularTexture);

	m_pSpecularTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gTextureSpecularIntensity", m_pSpecularTexture->GetShaderResourceView());
}

void PhongMaterial::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent*) const
{
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);
}
