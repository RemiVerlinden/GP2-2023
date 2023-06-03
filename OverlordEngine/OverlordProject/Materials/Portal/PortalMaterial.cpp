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

void PortalMaterial::OnUpdateModelVariables(const SceneContext&, const ModelComponent* /*pModel*/) const
{
	////  2. Update the portal texture
	RenderTarget* portalRenderTarget = PortalRenderer::Get()->GetPortalRenderTarget(m_Portal);
	SetVariable_Texture(L"gPortalMap", portalRenderTarget->GetColorShaderResourceView());
}

OrangePortalMaterial::OrangePortalMaterial()
	: PortalMaterial()
{
	m_Portal = PortalRenderer::Portal::blue; // we want to see the blue portal through the orange portal
}

BluePortalMaterial::BluePortalMaterial()
	: PortalMaterial()
{
	m_Portal = PortalRenderer::Portal::orange; // we want to see the orange portal through the blue portal
}
