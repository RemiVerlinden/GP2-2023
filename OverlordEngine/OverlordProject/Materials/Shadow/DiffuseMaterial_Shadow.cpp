#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow.fx")
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	//  1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	//  LightWVP = model_world * light_viewprojection
	//  (light_viewprojection[LightVP] can be acquired from the ShadowMapRenderer)

	XMMATRIX model_world = XMLoadFloat4x4(&pModel->GetTransform()->GetWorld());
	XMMATRIX light_viewprojection = XMLoadFloat4x4(&ShadowMapRenderer::Get()->GetLightVP());

	XMMATRIX LightWVP_matrix = model_world * light_viewprojection;
	XMFLOAT4X4 lightWVP;
	XMStoreFloat4x4(&lightWVP, LightWVP_matrix);

	SetVariable_Matrix(L"gWorldViewProj_Light", lightWVP);

	//  2. Update the ShadowMap texture
	SetVariable_Texture(L"gShadowMap", ShadowMapRenderer::Get()->GetShadowMap());

	// 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetLight(0).direction);
}
