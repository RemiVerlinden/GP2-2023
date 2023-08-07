#include "stdafx.h"
#include "CameraComponent.h"
CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(XM_PIDIV4),
	m_Size(25.0f),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

void CameraComponent::SetProjection(const XMFLOAT4X4& newProjection)
{
	SetProjection(XMLoadFloat4x4(&newProjection));
}

void CameraComponent::SetProjection(const XMMATRIX& newProjection)
{
	XMStoreFloat4x4(&m_Projection, newProjection);
}

GameObject* CameraComponent::Pick(CollisionGroup ignoreGroups) const
{
	return PickDetailed(ignoreGroups).first;
}

std::pair<GameObject*, CameraComponent::RaycastHit> CameraComponent::PickDetailed(CollisionGroup ignoreGroups) const
{
	//get mouse position
	const PxVec2 mousePos
	{
		static_cast<float>(InputManager::GetMousePosition().x),
		static_cast<float>(InputManager::GetMousePosition().y)
	};
	//get screen size
	const PxVec2 screenSize{ m_pScene->GetSceneContext().windowWidth ,m_pScene->GetSceneContext().windowHeight };

	//get world view and projection matrices
	XMMATRIX view, proj;
	view = XMLoadFloat4x4(&m_View);
	proj = XMLoadFloat4x4(&m_Projection);

	//create 2 points in screen space
	XMVECTOR nearPointScreen, farPointScreen;
	nearPointScreen = XMVectorSet(mousePos.x, mousePos.y, 0.0f, 1.f);
	farPointScreen = XMVectorSet(mousePos.x, mousePos.y, 1.f, 1.f);

	//unproject points to world space
	XMVECTOR nearPointWorld, farPointWorld;
	nearPointWorld = XMVector3Unproject(nearPointScreen, 0, 0, screenSize.x, screenSize.y, 0, 1, proj, view, XMMatrixIdentity());
	farPointWorld = XMVector3Unproject(farPointScreen, 0, 0, screenSize.x, screenSize.y, 0, 1, proj, view, XMMatrixIdentity());
	//create direction using 2 points
	XMVECTOR rayDirection;
	rayDirection = XMVector3Normalize(farPointWorld - nearPointWorld);

	//store SIMD variables in usable local variables
	XMFLOAT3 origin, direction;
	XMStoreFloat3(&origin, nearPointWorld);
	XMStoreFloat3(&direction, rayDirection);

	//define filter using ignored groups setting
	PxQueryFilterData filterData{};
	filterData.data.word0 = ~static_cast<UINT>(ignoreGroups);

	//if raycast hits something
	PxRaycastBuffer pxHit{};
	RaycastHit hit{};
	if (m_pScene->GetPhysxProxy()->Raycast(PxVec3(origin.x, origin.y, origin.z), PxVec3(direction.x, direction.y, direction.z), PX_MAX_F32, pxHit, PxHitFlag::eDEFAULT, filterData))
	{
		//get rigid body component from pxActor data
		RigidBodyComponent* rb = reinterpret_cast<RigidBodyComponent*>(pxHit.getAnyHit(0).actor->userData);

		{
			const PxRaycastHit& h = pxHit.getAnyHit(0);

			PxVec3 pos, normal;
			pos = h.position;
			normal = h.normal;


			hit.position = XMFLOAT3(pos.x, pos.y, pos.z);
			hit.normal = XMFLOAT3(normal.x, normal.y, normal.z);
			hit.distance = h.distance;
		}

		//get owning game object and return it
		return std::make_pair(rb->GetGameObject(), hit);
	}

	//return nullptr if no hit was found
	return std::make_pair((GameObject*)nullptr, hit);
}

std::pair<GameObject*, CameraComponent::RaycastHit> CameraComponent::CrosshairRaycast(CollisionGroup ignoreGroups) const
{
	// Calculate screen center
	const PxVec2 screenSize{ m_pScene->GetSceneContext().windowWidth ,m_pScene->GetSceneContext().windowHeight };
	const PxVec2 centerPos{ screenSize.x / 2.0f, screenSize.y / 2.0f };

	//get world view and projection matrices
	XMMATRIX view, proj;
	view = XMLoadFloat4x4(&m_View);
	proj = XMLoadFloat4x4(&m_Projection);

	//create 2 points in screen space
	XMVECTOR nearPointScreen, farPointScreen;
	nearPointScreen = XMVectorSet(centerPos.x, centerPos.y, 0.0f, 1.f);
	farPointScreen = XMVectorSet(centerPos.x, centerPos.y, 1.f, 1.f);

	//unproject points to world space
	XMVECTOR nearPointWorld, farPointWorld;
	nearPointWorld = XMVector3Unproject(nearPointScreen, 0, 0, screenSize.x, screenSize.y, 0, 1, proj, view, XMMatrixIdentity());
	farPointWorld = XMVector3Unproject(farPointScreen, 0, 0, screenSize.x, screenSize.y, 0, 1, proj, view, XMMatrixIdentity());

	//create direction using 2 points
	XMVECTOR rayDirection;
	rayDirection = XMVector3Normalize(farPointWorld - nearPointWorld);

	//store SIMD variables in usable local variables
	XMFLOAT3 origin, direction;
	XMStoreFloat3(&origin, nearPointWorld);
	XMStoreFloat3(&direction, rayDirection);

	//define filter using ignored groups setting
	PxQueryFilterData filterData{};
	filterData.data.word0 = ~static_cast<UINT>(ignoreGroups);

	//if raycast hits something
	PxRaycastBuffer pxHit{};
	RaycastHit hit{};
	if (m_pScene->GetPhysxProxy()->Raycast(PxVec3(origin.x, origin.y, origin.z), PxVec3(direction.x, direction.y, direction.z), PX_MAX_F32, pxHit, PxHitFlag::eDEFAULT, filterData))
	{
		//get rigid body component from pxActor data
		RigidBodyComponent* rb = reinterpret_cast<RigidBodyComponent*>(pxHit.getAnyHit(0).actor->userData);

		{
			const PxRaycastHit& h = pxHit.getAnyHit(0);

			PxVec3 pos, normal;
			pos = h.position;
			normal = h.normal;

			hit.position = XMFLOAT3(pos.x, pos.y, pos.z);
			hit.normal = XMFLOAT3(normal.x, normal.y, normal.z);
			hit.distance = h.distance;
		}

		//get owning game object and return it
		return std::make_pair(rb->GetGameObject(), hit);
	}

	//return nullptr if no hit was found
	return std::make_pair((GameObject*)nullptr, hit);
}


void CameraComponent::CalculateObliqueMatrix(DirectX::XMFLOAT4 inputClipPlaneCameraSpace)
{
	DirectX::XMVECTOR Q;
	DirectX::XMMATRIX ProjectionMatrix = XMLoadFloat4x4(&GetProjection());
	// Calculate the clip-space corner point opposite the clipping plane
	// as (sgn(clipPlaneCameraSpace.x), sgn(clipPlaneCameraSpace.y), 1, 1) and
	// transform it into camera space by multiplying it by the inverse of
	// the projection matrix
	XMVECTOR clipPlaneCameraSpace = XMLoadFloat4(&inputClipPlaneCameraSpace);

	Q = DirectX::XMVectorSet(
		(DirectX::XMVectorGetX(clipPlaneCameraSpace) < 0.f) ? -1.0f : 1.0f,
		(DirectX::XMVectorGetY(clipPlaneCameraSpace) < 0.f) ? -1.0f : 1.0f,
		1.0f, 1.0f);
	Q = DirectX::XMVector4Transform(Q, DirectX::XMMatrixInverse(nullptr, ProjectionMatrix));
	// Calculate the scaled plane vector
	DirectX::XMVECTOR clipPlane = clipPlaneCameraSpace;
	DirectX::XMVECTOR scaledPlane = DirectX::XMVectorScale(clipPlane, DirectX::XMVectorGetX(DirectX::XMVector4Dot(clipPlane, Q)));
	// Replace the third row of the projection matrix
	DirectX::XMMATRIX modifiedProj = ProjectionMatrix;
	modifiedProj.r[2] = DirectX::XMVectorAdd(modifiedProj.r[2], scaledPlane);

	SetProjection(modifiedProj);
}
