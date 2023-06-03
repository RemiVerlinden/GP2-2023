#include "stdafx.h"
#include "FrostedGlassMaterial.h"

FrostedGlassMaterial::FrostedGlassMaterial() :
	Material(L"Effects/Portal/FrostedGlassMaterial.fx")
{
}

void FrostedGlassMaterial::SetDecalTexture(const std::wstring& assetFile)
{
	m_pDecalTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gDecalTexture", m_pDecalTexture->GetShaderResourceView());
}

void FrostedGlassMaterial::InitializeEffectVariables()
{
}
