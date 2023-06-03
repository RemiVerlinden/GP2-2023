#include "stdafx.h"
#include "RefractingGlassMaterial.h"

RefractingGlassMaterial::RefractingGlassMaterial() :
	Material(L"Effects/Portal/RefractingGlassMaterial.fx")
{
}

void RefractingGlassMaterial::InitializeEffectVariables()
{
	auto pRefractingGlassTexture = ContentManager::Load<TextureData>(L"Textures/Maps/Chamber02_static/materials_glass_glass_normal.dds");

	SetVariable_Texture(L"gNormalTexture", pRefractingGlassTexture->GetShaderResourceView());
}
