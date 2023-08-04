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

void PortalComponent::UpdatePortalLifetime(const SceneContext& sceneContext)
{
	m_PortalLifetime += sceneContext.pGameTime->GetElapsed();
	if (m_HasMoved) // reset lifetime if portal has moved
	{
		m_PortalLifetime = 0.f;
		m_HasMoved = false;
	}
}

void PortalComponent::Update(const SceneContext& sceneContext)
{
	assert(m_pLinkedPortal);

	UpdatePortalLifetime(sceneContext);

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

	m_pPortalCameraHolder->GetTransform()->TranslateWorld(MatrixUtil::GetPositionFromMatrix(portalCamTransform));

	// Get the world rotations
	XMVECTOR PortalRotation = XMLoadFloat4(&m_pLinkedPortal->GetTransform()->GetWorldRotation()); // Assume this returns XMVECTOR
	XMVECTOR playerCamRotation = XMLoadFloat4(&playerCam->GetTransform()->GetWorldRotation()); // Assume this returns XMVECTOR

	// Compute the conjugate (inverse for unit quaternions) of the second rotation
	XMVECTOR invPlayerCamRotation = XMQuaternionConjugate(PortalRotation);


	// Compute the relative rotation
	XMVECTOR relativeRotation = XMQuaternionMultiply(playerCamRotation, invPlayerCamRotation);
	m_pPortalCameraRotator->GetTransform()->Rotate(relativeRotation);
}

void PortalComponent::PostUpdate(const SceneContext& /*sceneContext*/)
{
	// I dont understand why but this makes my portal cameras work, I havent looked at why this works yet
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
	int dot = CalculateDot(clipPlane->GetForward(), clipPlane->GetWorldPosition(), m_pPortalCam->GetTransform()->GetWorldPosition());
	// To multiply a point or vector by a matrix in DirectX, we can use XMVector3TransformCoord or XMVector3TransformNormal
	XMMATRIX worldToCameraMatrix = DirectX::XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_pPortalCam->GetView()));

	XMVECTOR camSpacePos = XMVector3TransformCoord(XMLoadFloat3(&clipPlane->GetWorldPosition()), worldToCameraMatrix);
	XMVECTOR camSpaceNormal = XMVector3TransformNormal(XMLoadFloat3(&clipPlane->GetForward()), worldToCameraMatrix) * static_cast<float>(dot);
	float camSpaceDst = -XMVectorGetX(XMVector3Dot(camSpacePos, camSpaceNormal)) + m_NearClipPlaneOffset;

	std::cout << camSpaceDst << std::endl;

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
