#include "stdafx.h"
#include "ShadowMapMaterial.h"

ShadowMapMaterial::ShadowMapMaterial():
	Material(L"Effects/Shadow/ShadowMapGenerator.fx")
{
}

void ShadowMapMaterial::InitializeEffectVariables()
{
}

void ShadowMapMaterial::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent*) const
{
	SetVariable_Vector(L"gLightPosition", sceneContext.pLights->GetLight(0).position);
}
