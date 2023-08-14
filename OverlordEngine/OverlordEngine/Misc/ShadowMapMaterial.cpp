#include "stdafx.h"
#include "ShadowMapMaterial.h"

ShadowMapMaterial::ShadowMapMaterial():
	Material(L"Effects/Shadow/ShadowMapGenerator.fx")
{
}

void ShadowMapMaterial::InitializeEffectVariables()
{
	SetVariable_Scalar(L"gNearPlane", ShadowMapRendererCube::GetNearPlane());
	SetVariable_Scalar(L"gFarPlane", ShadowMapRendererCube::GetFarPlane());
}

void ShadowMapMaterial::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent*) const
{
	SetVariable_Vector(L"gLightPosition", sceneContext.pLights->GetLight(0).position);
}
