#include "stdafx.h"
#include "PortalMaterial.h"
#include "Graphics\CameraViewMapRenderer.h"
#include "Components\PortalComponent.h"
#include "../OverlordEngine/Graphics/PortalRenderer.h"

PortalMaterial::PortalMaterial() :
	Material(L"Effects/Portal/Portal.fx")
{
}

void PortalMaterial::SetPortalComponent(PortalComponent* pComponent)
{
	m_pPortalComponent = pComponent;
}

void PortalMaterial::InitializeEffectVariables()
{
}

void PortalMaterial::OnUpdateModelVariables(const SceneContext& /*sceneContext*/, const ModelComponent* /*pModel*/) const
{
	////  2. Update the portal texture
	RenderTarget* portalRenderTarget = PortalRenderer::Get()->GetPortalRenderTarget(m_Portal);
	SetVariable_Texture(L"gPortalMap", portalRenderTarget->GetColorShaderResourceView());

	SetVariable_Scalar(L"gTime", m_pPortalComponent->GetPortalLifetime());
}
//=======================================================================================================================================

OrangePortalMaterial::OrangePortalMaterial()
	: PortalMaterial()
{
	m_Portal = PortalRenderer::Portal::blue; // we want to see the blue portal through the orange portal
}

void OrangePortalMaterial::InitializeEffectVariables()
{
	// You can add custom initialization code here.
	PortalMaterial::InitializeEffectVariables();
	SetVariable_Scalar(L"gBluePortalColor", false);
}
//=======================================================================================================================================

BluePortalMaterial::BluePortalMaterial()
	: PortalMaterial()
{
	m_Portal = PortalRenderer::Portal::orange; // we want to see the orange portal through the blue portal

}

void BluePortalMaterial::InitializeEffectVariables()
{
	// You can add custom initialization code here.
	PortalMaterial::InitializeEffectVariables();
	SetVariable_Scalar(L"gBluePortalColor", true);

}
//=======================================================================================================================================
