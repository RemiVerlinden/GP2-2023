#include "stdafx.h"
#include "SkyBoxmaterial.h"

SkyBoxmaterial::SkyBoxmaterial() :
	Material(L"Effects/SkyBox.fx")
{
}

void SkyBoxmaterial::InitializeEffectVariables()
{
}


void SkyBoxmaterial::SetSkyBoxTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture->GetShaderResourceView());
}


void SkyBoxmaterial::OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const
{
		SetVariable_Texture(L"gDiffuseMap", ShadowMapRendererCube::Get()->GetShadowMap(1));
}