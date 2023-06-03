#include "stdafx.h"
#include "NoDrawMaterial.h"

NoDrawMaterial::NoDrawMaterial()
	:Material<NoDrawMaterial>(L"Effects/Portal/NoDrawMaterial.fx")
{
}

void NoDrawMaterial::SetDebugRendering(bool setDebugDraw) // if this is enabled, then a debug texture is drawn instead of nothingness
{
	SetVariable_Scalar(L"gDebugRendering", setDebugDraw);
}

void NoDrawMaterial::InitializeEffectVariables()
{
	auto pNoDrawTexture = ContentManager::Load<TextureData>(L"Textures/Dev/nodraw.dds");

	SetVariable_Texture(L"gDiffuseMap", pNoDrawTexture->GetShaderResourceView());
}