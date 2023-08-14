#include "stdafx.h"
#include "DecalMaterial.h"

DecalMaterial::DecalMaterial() : 
	Material(L"Effects/Portal/DecalMaterial.fx")
{
}

void DecalMaterial::SetDecalTexture(const std::wstring& assetFile)
{
	m_pDecalTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gDecalTexture", m_pDecalTexture->GetShaderResourceView());
}

void DecalMaterial::InitializeEffectVariables()
{
	m_EnableShadows = true;
}
