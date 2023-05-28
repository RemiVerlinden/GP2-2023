#include "stdafx.h"
#include "TransformComponent.h"

TransformComponent::TransformComponent() :
	m_Position{ 0, 0, 0 },
	m_WorldPosition{ 0, 0, 0 },
	m_Scale{ 1, 1, 1 },
	m_WorldScale{ 1, 1, 1 },
	m_Forward{ 0, 0, 1 },
	m_Up{ 0, 1, 0 },
	m_Right{ 1, 0, 0 },
	m_Rotation{ 0, 0, 0, 1 },
	m_WorldRotation{ 0, 0, 0, 1 }
{
}

bool TransformComponent::CheckIfDirty()
{
	//If Parent is dirty == update required (spatial relation)
	if (const GameObject* pParent = GetGameObject()->GetParent())
	{
		if (pParent->GetTransform()->IsDirty())
		{
			m_IsDirty = true;
			return true;
		}
	}

	//RigidBody (non static), Controller or Transform changed == update required
	m_IsDirty = m_pRigidBodyComponent != nullptr && !m_pRigidBodyComponent->IsStatic();
	m_IsDirty = m_IsDirty || m_pControllerComponent != nullptr;
	m_IsDirty = m_IsDirty || m_IsTransformChanged != TransformChanged::NONE;

	return m_IsDirty;
}

void TransformComponent::Initialize(const SceneContext&)
{
	UpdateTransforms();
}

void TransformComponent::Update(const SceneContext&)
{
	m_IsDirty = CheckIfDirty();

	if (m_IsDirty)
		UpdateTransforms();
}

void TransformComponent::UpdateTransforms()
{
	ASSERT_IF(m_pRigidBodyComponent && m_pControllerComponent, L"Single GameObject can't have a RigidBodyComponent AND ControllerComponent at the same time (remove one)")

		if (m_pRigidBodyComponent && m_IsInitialized)
		{
			if (isSet(m_IsTransformChanged, TransformChanged::TRANSLATION))m_pRigidBodyComponent->Translate(m_Position);
			else m_Position = m_pRigidBodyComponent->GetPosition();

			if (isSet(m_IsTransformChanged, TransformChanged::ROTATION)) m_pRigidBodyComponent->Rotate(m_Rotation);
			else m_Rotation = m_pRigidBodyComponent->GetRotation();
		}
		else if (m_pControllerComponent && m_IsInitialized)
		{
			if (isSet(m_IsTransformChanged, TransformChanged::TRANSLATION)) m_pControllerComponent->Translate(m_Position);
			else m_Position = m_pControllerComponent->GetPosition();
		}

	//Calculate World Matrix
	//**********************
	auto rot = XMLoadFloat4(&m_Rotation);
	auto world = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
		XMMatrixRotationQuaternion(rot) *
		XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

	if (const auto pParent = m_pGameObject->GetParent())
	{
		const auto parentWorld = XMLoadFloat4x4(&pParent->GetTransform()->m_World);
		world *= parentWorld;
	}

	XMStoreFloat4x4(&m_World, world);

	//Get World Transform
	XMVECTOR pos, scale;
	if (XMMatrixDecompose(&scale, &rot, &pos, world))
	{
		XMStoreFloat3(&m_WorldPosition, pos);
		XMStoreFloat3(&m_WorldScale, scale);
		XMStoreFloat4(&m_WorldRotation, rot);
	}

	const auto rotMat = XMMatrixRotationQuaternion(rot);
	const auto forward = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), rotMat);
	const auto right = XMVector3TransformCoord(XMVectorSet(1, 0, 0, 0), rotMat);
	const auto up = XMVector3Cross(forward, right);

	XMStoreFloat3(&m_Forward, forward);
	XMStoreFloat3(&m_Right, right);
	XMStoreFloat3(&m_Up, up);

	m_IsTransformChanged = TransformChanged::NONE;
}

void TransformComponent::Translate(float x, float y, float z)
{
	//if (!CheckConstraints())
		//return;

	m_IsTransformChanged |= TransformChanged::TRANSLATION;
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void TransformComponent::Translate(const XMFLOAT3& position)
{
	Translate(position.x, position.y, position.z);
}

void TransformComponent::Translate(const XMVECTOR& position)
{
	//if (!CheckConstraints())
		//return;

	m_IsTransformChanged |= TransformChanged::TRANSLATION;
	XMStoreFloat3(&m_Position, position);
}

void TransformComponent::TranslateWorld(const XMFLOAT3& position)
{
	XMVECTOR newPos = XMLoadFloat3(&position);
	if (const auto pParent = m_pGameObject->GetParent())
	{
		// Convert them to XMMATRIX and XMVECTOR.
		XMMATRIX inverseMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&pParent->GetTransform()->GetWorld()));
		DirectX::XMVECTOR posVector = XMLoadFloat3(&position);

		// Extend posVector to 4 dimensions.
		DirectX::XMVECTOR posVector4D = DirectX::XMVectorSetW(posVector, 1.0f);

		// Apply the transformation.
		newPos = DirectX::XMVector4Transform(posVector4D, inverseMatrix);

		//DirectX::XMStoreFloat3(&newPosition, resultVector);
	}
	Translate(newPos);
}

void TransformComponent::Rotate(float x, float y, float z, bool degrees)
{
	//if (!CheckConstraints())
		//return;

	m_IsTransformChanged |= TransformChanged::ROTATION;
	if (degrees)
	{
		XMStoreFloat4(&m_Rotation,
			XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x),
				XMConvertToRadians(y),
				XMConvertToRadians(z)));
	}
	else
	{
		XMStoreFloat4(&m_Rotation, XMQuaternionRotationRollPitchYaw(x, y, z));
	}
}

void TransformComponent::Rotate(const XMFLOAT3& rotation, bool areDegrees)
{
	Rotate(rotation.x, rotation.y, rotation.z, areDegrees);
}

void TransformComponent::Rotate(const XMVECTOR& rotation, bool isQuaternion)
{
	//if (!CheckConstraints())
		//return;

	m_IsTransformChanged |= TransformChanged::ROTATION;
	if (isQuaternion)
	{
		XMStoreFloat4(&m_Rotation, rotation);
	}
	else
	{
		Logger::LogTodo(L"TransformComponent::Rotate(XMVECTOR, non-Quad) > Not Implemented Yet!");
	}
}

void TransformComponent::Rotate(const XMFLOAT4& rotation, bool isQuaternion)
{
	Rotate(XMLoadFloat4(&rotation), isQuaternion);
}

void TransformComponent::RotateWorld(const XMVECTOR& rotation)
{
	XMVECTOR relativeRot = rotation;
	if (const auto pParent = m_pGameObject->GetParent())
	{
		XMFLOAT4 parentRot = pParent->GetTransform()->GetWorldRotation();
		XMVECTOR parentInverse = XMQuaternionInverse(XMLoadFloat4(&parentRot));
		relativeRot = DirectX::XMQuaternionMultiply(parentInverse, rotation);
	}
	Rotate(relativeRot, true);
}

void TransformComponent::SetTransform(const XMFLOAT4X4& transform)
{
	XMVECTOR pos, rot, scale;

	if (XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&transform)))
	{
		XMFLOAT4 XM_pos, XM_scale;
		XMStoreFloat4(&XM_pos, pos);
		XMStoreFloat4(&XM_scale, scale);

		TranslateWorld({ XM_pos.x,XM_pos.y,XM_pos.z });
		RotateWorld(rot);
		Scale(XM_scale.x, XM_scale.y, XM_scale.z);
	}
}


void TransformComponent::Scale(float x, float y, float z)
{
	//if (!CheckConstraints())
		//return;

	m_IsTransformChanged |= TransformChanged::SCALE;
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;
}

void TransformComponent::Scale(float s)
{
	Scale(s, s, s);
}

void TransformComponent::Scale(const XMFLOAT3& scale)
{
	Scale(scale.x, scale.y, scale.z);
}

XMFLOAT4X4 TransformComponent::GetWorldToLocal() const
{
	XMFLOAT4X4 worldToLocal;
	XMStoreFloat4x4(&worldToLocal, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_World)));
	return worldToLocal;
}

bool TransformComponent::CheckConstraints() const
{
	return true;
	//if (!m_IsInitialized)
	//	return true;

	//const auto rigidBody = GetGameObject()->GetComponent<RigidBodyComponent>();
	//if (rigidBody != nullptr && rigidBody->IsStatic())
	//{
	//	Logger::LogWarning(L"[TransformComponent] Constraint Broken: GameObject with a static rigid body can't be transformed!");
	//	//return false;
	//}

	//return true;
}