#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial()
	:Material<UberMaterial>(L"Labs/Week4/Effects/UberShader.fx")
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

void UberMaterial::InitializeEffectVariables()
{
	SetVariable_Vector(L"gLightDirection", XMFLOAT3{ 0.577f, 0.577f, -0.577f });
	SetVariable_Vector(L"gColorDiffuse", XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f });
	SetVariable_Vector(L"gColorSpecular", XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f });
	SetVariable_Vector(L"gColorAmbient", XMFLOAT4{ 0.5f, 0.6f, 0.9f, 1.0f });

	SetVariable_Scalar(L"gUseTextureDiffuse", true);
	SetVariable_Scalar(L"gUseTextureSpecularIntensity", true);
	SetVariable_Scalar(L"gUseTextureNormal", true);
	SetVariable_Scalar(L"gUseEnvironmentMapping", true);
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Scalar(L"gUseSpecularPhong", true);

	SetVariable_Scalar(L"gShininess", 6);
	SetVariable_Scalar(L"gAmbientIntensity", 0.1f);
	SetVariable_Scalar(L"gReflectionStrength", 0.7f);
	SetVariable_Scalar(L"gRefractionStrength", 0.2f);
	SetVariable_Scalar(L"gRefractionIndex", 1.0f);

	SetDiffuseTexture(L"Labs/Week3/Textures/Skulls Textures/Skulls_DiffuseMap.tga");
	SetSpecularIntensityTexture(L"Labs/Week3/Textures/Skulls Textures/Skulls_HeightMap.tga");
	SetOpacityTexture(L"Labs/Week3/Textures/Skulls Textures/Skulls_HeightMap.tga");
	SetNormalMapTexture(L"Labs/Week3/Textures/Skulls Textures/Skulls_NormalMap.tga");
	SetCubeEnvironmentTexture(L"Labs/Week3/Textures/Sunol_Cubemap.dds");
}
