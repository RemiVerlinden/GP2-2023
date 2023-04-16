#include "stdafx.h"
#include "SkullBallUberMaterial.h"

SkullBallUberMaterial::SkullBallUberMaterial()
	:UberMaterial()
{
}

void SkullBallUberMaterial::InitializeEffectVariables()
{
	SetVariable_Vector(L"gLightDirection", XMFLOAT3{ 0.551f, -0.534f, 0.641f });
	SetVariable_Vector(L"gColorDiffuse", XMFLOAT4{ 1, 0.101f, 0.141f, 1.0f });
	SetVariable_Vector(L"gColorSpecular", XMFLOAT4{ 0, 0.796f, 0.250f, 1.0f });
	SetVariable_Vector(L"gColorAmbient", XMFLOAT4{ 0.5f, 0.6f, 0.9f, 1.0f });

	SetVariable_Scalar(L"gUseTextureDiffuse", true);
	SetVariable_Scalar(L"gUseTextureSpecularIntensity", true);
	SetVariable_Scalar(L"gUseTextureNormal", true);
	SetVariable_Scalar(L"gUseEnvironmentMapping", true);
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Scalar(L"gUseSpecularPhong", true);

	SetVariable_Scalar(L"gShininess", 40);
	SetVariable_Scalar(L"gAmbientIntensity", 0.0f);

	SetVariable_Scalar(L"gReflectionStrength", 0.8f);
	SetVariable_Scalar(L"gRefractionStrength", 0.5f);
	SetVariable_Scalar(L"gRefractionIndex", 0.9f);

	SetDiffuseTexture(L"Labs/Week3/Textures/Skulls Textures/Skulls_DiffuseMap.tga");
	SetSpecularIntensityTexture(L"Labs/Week3/Textures/Skulls Textures/Skulls_HeightMap.tga");
	SetOpacityTexture(L"Labs/Week3/Textures/Skulls Textures/Skulls_HeightMap.tga");
	SetNormalMapTexture(L"Labs/Week3/Textures/Skulls Textures/Skulls_NormalMap.tga");
	SetCubeEnvironmentTexture(L"Labs/Week3/Textures/Sunol_Cubemap.dds");
}
