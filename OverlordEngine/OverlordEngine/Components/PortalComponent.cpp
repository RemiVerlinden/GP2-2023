// PortalComponent.cpp
#include "stdafx.h"
#include "PortalComponent.h"
#include "Graphics/CameraViewMapRenderer.h"
#include "Components\ControllerComponent.h"
PortalComponent::PortalComponent(CameraComponent* playerCamera, bool color)
	: m_pLinkedPortal(nullptr)
	, m_pPlayerCam(playerCamera)
	, m_pPortalCam(nullptr)
	,m_pPortalCameraHolder(nullptr)
{
	this->bluePortal = color;
}

PortalComponent::~PortalComponent()
{
}

void PortalComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	// Add your initialization code here
	m_pPortalCameraHolder = m_pGameObject->AddChild(new GameObject);
	m_pPortalCameraRotator = m_pPortalCameraHolder->AddChild(new GameObject);

	m_pPortalCam = m_pPortalCameraRotator->AddComponent(new CameraComponent());
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
	assert(m_pLinkedPortal);
	//m_pGameObject->GetScene()->SetActiveCamera(m_pPortalCam);

	//CameraViewMapRenderer::Get()->Begin(sceneContext, m_pPortalCam);






	CameraComponent* playerCam = m_pPlayerCam;

	XMFLOAT4X4 portalLocalToWorld, linkedPortalWorldToLocal, playerCamLocalToWorld, portalCamTransform;
	XMMATRIX portalLocalToWorldMatrix, linkedPortalWorldToLocalMatrix, playerCamLocalToWorldMatrix, portalCamTransformMatrix;

	playerCamLocalToWorld = playerCam->GetTransform()->GetLocalToWorld();
	linkedPortalWorldToLocal = m_pLinkedPortal->GetTransform()->GetWorldToLocal();
	portalLocalToWorld = GetTransform()->GetLocalToWorld();

	// Load into XMMATRIX
	portalLocalToWorldMatrix = XMLoadFloat4x4(&portalLocalToWorld);
	linkedPortalWorldToLocalMatrix = XMLoadFloat4x4(&linkedPortalWorldToLocal);
	playerCamLocalToWorldMatrix = XMLoadFloat4x4(&playerCamLocalToWorld);

	portalCamTransformMatrix = playerCamLocalToWorldMatrix * linkedPortalWorldToLocalMatrix * portalLocalToWorldMatrix;


	XMStoreFloat4x4(&portalCamTransform, portalCamTransformMatrix);
	//MatrixUtil::MultiplyMatrices(portalLocalToWorld, linkedPortalWorldToLocal, playerCamLocalToWorld, portalCamTransform);

	m_pPortalCameraHolder->GetTransform()->TranslateWorld(MatrixUtil::GetPositionFromMatrix(portalCamTransform));
	m_pPortalCameraRotator->GetTransform()->RotateWorld(MatrixUtil::GetRotationFromMatrix(portalCamTransform));

 	//m_pPortalCameraHolder->GetTransform()->SetTransform(portalCamTransform);

	//XMFLOAT3 localToWorld = MatrixUtil::GetPositionFromMatrix(portalCamTransform);
	//static int counter = 0;
	//std::wcout << std::format(L"frame {} ----- {:.1f} \t| {:.1f} \t| {:.1f}",counter, localToWorld.x, localToWorld.y, localToWorld.z) << std::endl;
	//++counter;
	//CameraComponent* playerCamera = m_pGameObject->GetScene()->GetActiveCamera();

	//XMFLOAT3 pos1 = playerCamera->GetTransform()->GetWorldPosition();
	//XMFLOAT3 pos2 = MatrixUtil::GetPositionFromMatrix(playerCamera->GetTransform()->GetLocalToWorld());



	//m_pPortalCameraHolder->GetTransform()->Translate(playerCamera->GetTransform()->GetWorldPosition());
	//m_pPortalCameraHolder->GetTransform()->Rotate(playerCamera->GetTransform()->GetWorldRotation());
}

void PortalComponent::PortalMapDraw(const SceneContext& /*context*/)
{

	std::cout << "this shouldnt happen" << std::endl;
	
}

void PortalComponent::PrePortalRender(const SceneContext& /*context*/)
{
	std::cout << "this shouldnt happen" << std::endl;
}

void PortalComponent::Render(const SceneContext&)
{
	// Add your Render code here

	//m_pScene->PortalDraw();
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

