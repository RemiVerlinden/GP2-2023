#include "stdafx.h"
#include "FrostedGlassMaterial.h"

FrostedGlassMaterial::FrostedGlassMaterial() :
	Material(L"Effects/Portal/FrostedGlassMaterial.fx")
{
}

void FrostedGlassMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gDiffuseTexture", m_pDiffuseTexture->GetShaderResourceView());
}

void FrostedGlassMaterial::SetOpacity(const float opacity)
{
	m_Opacity = opacity;
	SetVariable_Scalar(L"gOpacity", m_Opacity);
}

void FrostedGlassMaterial::InitializeEffectVariables()
{
}
