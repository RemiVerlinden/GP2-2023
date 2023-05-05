#include "stdafx.h"
#include "PortalMaterial.h"
#include "Graphics\CameraViewMapRenderer.h"
#include "Components\PortalComponent.h"
PortalMaterial::PortalMaterial() :
	Material(L"Effects/Portal/PosTex3d_Portal.fx")
{
}

void PortalMaterial::InitializeEffectVariables()
{
}

void PortalMaterial::OnUpdateModelVariables(const SceneContext&, const ModelComponent* pModel) const
{
	//  1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	//  LightWVP = model_world * light_viewprojection
	//  (light_viewprojection[LightVP] can be acquired from the ShadowMapRenderer)

	static PortalComponent* pPortalComponent = pModel->GetGameObject()->GetComponent<PortalComponent>();

	XMMATRIX model_world = XMLoadFloat4x4(&pModel->GetTransform()->GetWorld());
	XMMATRIX PortalCamera_viewprojection = XMLoadFloat4x4(&pPortalComponent->GetPortalCamera()->GetViewProjection());

	XMMATRIX PortalCameraWVP_matrix = model_world * PortalCamera_viewprojection;
	XMFLOAT4X4 PortalCameraWVP;
	XMStoreFloat4x4(&PortalCameraWVP, PortalCameraWVP_matrix);

	SetVariable_Matrix(L"gWorldViewProj", PortalCameraWVP);

	//  2. Update the ShadowMap texture
	SetVariable_Texture(L"gPortalMap", CameraViewMapRenderer::Get()->GetColorMap());
}
