#include "stdafx.h"
#include "StaticMapMaterial.h"

StaticMapMaterial::StaticMapMaterial() :
	Material(L"Effects/Portal/StaticMapMaterial.fx")
{
}

void StaticMapMaterial::InitializeEffectVariables()
{
}

void StaticMapMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture->GetShaderResourceView());
}