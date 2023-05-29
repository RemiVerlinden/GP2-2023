// PortalComponent.cpp
#include "stdafx.h"
#include "PortalComponent.h"
#include "Graphics/CameraViewMapRenderer.h"
#include "Components\ControllerComponent.h"
#include "../OverlordProject/Prefabs/Character.h"
PortalComponent::PortalComponent(Character* playerCharacter, bool color)
	: m_pLinkedPortal(nullptr)
	, m_pPlayerCharacter(playerCharacter)
	, m_pPlayerCam(playerCharacter->GetCameraComponent())
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



void PortalComponent::Update(const SceneContext& /*sceneContext*/)
{
	//HandleTravellers();
	assert(m_pLinkedPortal);
	//m_pGameObject->GetScene()->SetActiveCamera(m_pPortalCam);

	//CameraViewMapRenderer::Get()->Begin(sceneContext, m_pPortalCam);






	CameraComponent* playerCam = m_pPlayerCam;

	XMFLOAT4X4 portalLocalToWorld, linkedPortalWorldToLocal, playerCamLocalToWorld, portalCamTransform;
	XMMATRIX portalLocalToWorldMatrix, linkedPortalWorldToLocalMatrix, playerCamLocalToWorldMatrix, portalCamTransformMatrix;

	playerCamLocalToWorld = playerCam->GetLocalToWorldMatrix();
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

	// Get the world rotations
	XMVECTOR PortalRotation = XMLoadFloat4(&m_pLinkedPortal->GetTransform()->GetWorldRotation()); // Assume this returns XMVECTOR
	XMVECTOR playerCamRotation = XMLoadFloat4(&playerCam->GetTransform()->GetWorldRotation()); // Assume this returns XMVECTOR

	// Compute the conjugate (inverse for unit quaternions) of the second rotation
	XMVECTOR invPlayerCamRotation = XMQuaternionConjugate(PortalRotation);


	// Compute the relative rotation
	XMVECTOR relativeRotation = XMQuaternionMultiply(playerCamRotation, invPlayerCamRotation);
	m_pPortalCameraRotator->GetTransform()->Rotate(relativeRotation);


	//m_pPortalCameraRotator->GetTransform()->RotateWorld(MatrixUtil::GetRotationFromMatrix(portalCamTransform));
	//m_pPortalCameraRotator->GetTransform()->Rotate(m_pPlayerCharacter->GetPitch(), m_pPlayerCharacter->GetYaw(), 0);

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

void PortalComponent::PostUpdate(const SceneContext& /*sceneContext*/)
{
	m_pPortalCam->SetProjection(m_pPlayerCam->GetProjection());

	
	//HandlePortalCameraClipPlane();
}

void PortalComponent::PortalMapDraw(const SceneContext& /*context*/)
{

	std::cout << "this shouldnt happen" << std::endl;
	
}

inline static int CalculateDot(const XMFLOAT3& inputClipPlaneForward, const XMFLOAT3& inputTransformPosition, const XMFLOAT3& inputPortalCamPosition)
{
	// Assuming clipPlaneForward, transformPosition and portalCamPosition are DirectX::XMVECTOR or DirectX::XMFLOAT3 objects
	DirectX::XMVECTOR clipPlaneForward = XMLoadFloat3(&inputClipPlaneForward);
	DirectX::XMVECTOR transformPosition = XMLoadFloat3(&inputTransformPosition);
	DirectX::XMVECTOR portalCamPosition = XMLoadFloat3(&inputPortalCamPosition);

	// Subtract portalCamPosition from transformPosition
	DirectX::XMVECTOR difference = DirectX::XMVectorSubtract(transformPosition, portalCamPosition);

	// Compute dot product
	DirectX::XMVECTOR dotProduct = DirectX::XMVector3Dot(clipPlaneForward, difference);

	// Extract the first component of the dotProduct XMVECTOR to a float
	float dotProductFloat = DirectX::XMVectorGetX(dotProduct);

	// Determine the sign
	int dot = (dotProductFloat > 0) - (dotProductFloat < 0);

	return dot;
};

void PortalComponent::HandlePortalCameraClipPlane()
{
	// Learning resource:
	// http://www.terathon.com/lengyel/Lengyel-Oblique.pdf

	TransformComponent* clipPlane = GetTransform();  // Assuming GetTransform() returns the object's TransformComponent
	int dot = CalculateDot(clipPlane->GetForward(), clipPlane->GetPosition(), m_pPortalCam->GetTransform()->GetPosition());

	// To multiply a point or vector by a matrix in DirectX, we can use XMVector3TransformCoord or XMVector3TransformNormal
	DirectX::XMMATRIX worldToCameraMatrix = DirectX::XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_pPortalCam->GetView()));

	DirectX::XMVECTOR camSpacePos = DirectX::XMVector3TransformCoord(XMLoadFloat3(&clipPlane->GetPosition()), worldToCameraMatrix);
	DirectX::XMVECTOR camSpaceNormal = DirectX::XMVector3TransformNormal(XMLoadFloat3(&clipPlane->GetForward()), worldToCameraMatrix) * static_cast<float>(dot);
	float camSpaceDst = -XMVectorGetX(DirectX::XMVector3Dot(camSpacePos, camSpaceNormal)) + m_NearClipPlaneOffset;

	// Don't use oblique clip plane if very close to portal as it seems this can cause some visual artifacts
	if (fabs(camSpaceDst) > m_NearClipPlaneLimit)
	{
		DirectX::XMFLOAT4 clipPlaneCameraSpace;
		DirectX::XMStoreFloat4(&clipPlaneCameraSpace, camSpaceNormal);
		clipPlaneCameraSpace.w = camSpaceDst;

		// Update projection based on new clip plane
		// Calculate matrix with player cam so that player camera settings (fov, etc) are used
		m_pPortalCam->CalculateObliqueMatrix(clipPlaneCameraSpace);  // You will need to implement CalculateObliqueMatrix yourself
	}
	else
	{
		m_pPortalCam->SetProjection(m_pPlayerCam->GetProjection());
	}
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

