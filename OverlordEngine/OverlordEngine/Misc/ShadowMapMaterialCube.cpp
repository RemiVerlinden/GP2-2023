#include "stdafx.h"
#include "ShadowMapMaterialCube.h"

ShadowMapMaterialCube::ShadowMapMaterialCube():
	Material(L"Effects/Shadow/ShadowMapGeneratorCube.fx")
{
}

void ShadowMapMaterialCube::InitializeEffectVariables()
{
}
