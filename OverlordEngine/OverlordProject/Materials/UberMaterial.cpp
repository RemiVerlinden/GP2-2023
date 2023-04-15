#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial()
	:Material<UberMaterial>(L"Effects/UberShader.fx")
{
}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	SetVariable_Texture(L"gTextureDiffuse", ContentManager::Load<TextureData>(assetFile));
}

void UberMaterial::SetNormalMapTexture(const std::wstring& assetFile)
{
	SetVariable_Texture(L"gTextureNormal", ContentManager::Load<TextureData>(assetFile));
}

void UberMaterial::SetSpecularIntensityTexture(const std::wstring& assetFile)
{
	SetVariable_Texture(L"gTextureSpecularIntensity", ContentManager::Load<TextureData>(assetFile));
}

void UberMaterial::SetOpacityTexture(const std::wstring& assetFile)
{
	SetVariable_Texture(L"gTextureOpacity", ContentManager::Load<TextureData>(assetFile));
}

void UberMaterial::SetCubeEnvironmentTexture(const std::wstring& assetFile)
{
	SetVariable_Texture(L"gCubeEnvironment", ContentManager::Load<TextureData>(assetFile));
}
