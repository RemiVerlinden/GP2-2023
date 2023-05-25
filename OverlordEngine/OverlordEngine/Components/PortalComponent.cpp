// PortalComponent.cpp
#include "stdafx.h"
#include "PortalComponent.h"
#include "Graphics/CameraViewMapRenderer.h"

PortalComponent::PortalComponent()
	: m_pLinkedPortal(nullptr)
	, m_pPlayerCam(nullptr)
	, m_pPortalCam(nullptr)
	,m_pPortalCameraObject(nullptr)
{

}

PortalComponent::~PortalComponent()
{
}

void PortalComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	// Add your initialization code here
	m_pPortalCameraObject = m_pGameObject->AddChild(new GameObject);

	m_pPortalCam = m_pPortalCameraObject->AddComponent(new CameraComponent());
}

void PortalComponent::PreDraw(const SceneContext& context)
{
	PrePortalRender(context);
	Render(context);
	PostPortalRender(context);
}

void PortalComponent::Update(const SceneContext& /*sceneContext*/)
{
	//HandleTravellers();
}

void PortalComponent::PortalMapDraw(const SceneContext& /*context*/)
{

	
}

void PortalComponent::PrePortalRender(const SceneContext& context)
{
	assert(m_pLinkedPortal);
	m_pPlayerCam = m_pGameObject->GetScene()->GetActiveCamera();
	m_pGameObject->GetScene()->SetActiveCamera(m_pPortalCam);

	CameraViewMapRenderer::Get()->Begin(context, m_pPortalCam);

	CameraComponent* playerCam = m_pPlayerCam;

	XMFLOAT4X4 portalLocalToWorld, linkedPortalWorldToLocal, playerCamLocalToWorld, portalCamTransform;
	playerCamLocalToWorld = playerCam->GetLocalToWorldMatrix();
	linkedPortalWorldToLocal = m_pLinkedPortal->GetTransform()->GetWorldToLocal();
	portalLocalToWorld = GetTransform()->GetLocalToWorld();

	MatrixUtil::MultiplyMatrices(playerCamLocalToWorld, linkedPortalWorldToLocal, portalLocalToWorld, portalCamTransform);

	m_pPortalCameraObject->GetTransform()->Translate(MatrixUtil::GetPositionFromMatrix(portalCamTransform));
	m_pPortalCameraObject->GetTransform()->Rotate(MatrixUtil::GetRotationFromMatrix(portalCamTransform));
}

void PortalComponent::Render(const SceneContext&)
{
	// Add your Render code here

	m_pScene->PortalDraw();
}

void PortalComponent::PostPortalRender(const SceneContext& context)
{
	// Add your PostPortalRender code here
	CameraViewMapRenderer::Get()->End(context);
	m_pGameObject->GetScene()->SetActiveCamera(m_pPlayerCam);

}

void PortalComponent::SetLinkedPortal(PortalComponent* pPortal)
{
	m_pLinkedPortal = pPortal;
}

//void PortalComponent::OnTriggerEnter(Collider* other)
//{
//	// Add your OnTriggerEnter code here
//}
//
//void PortalComponent::OnTriggerExit(Collider* other)
//{
//	// Add your OnTriggerExit code here
//}

