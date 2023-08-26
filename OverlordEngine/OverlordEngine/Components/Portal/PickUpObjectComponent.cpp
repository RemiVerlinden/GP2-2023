#include "stdafx.h"
#include "PickUpObjectComponent.h"
#include "../OverlordProject/Prefabs/Character.h"

void PickUpObjectComponent::SetGrabbed(bool grabbed)
{
	m_IsGrabbed = grabbed;
	if (m_IsGrabbed)
	{
		SoundManager::Get()->Play2DSound(m_pSounds[SoundLibrary::Grab], 0.5f, SoundManager::SoundChannel::Level, false);
		SoundManager::Get()->Play3DSound(m_pSounds[GetRandomSound(impactType::soft)], 1.f, GetTransform()->GetWorldPosition(), SoundManager::SoundChannel::Level, true);
	}
}

void PickUpObjectComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	m_ImpactThreshholdSoft = 2.f;
	m_ImpactThreshholdHard = 4.f;

	if (m_pSounds.size() == 0)
	{
		const auto& soundManager = SoundManager::Get();

		std::string grabSound = "Resources/Sounds/Generic/pickup.wav";
		std::string impactSoft1Sound = "Resources/Sounds/Cube/impact_soft1.wav";
		std::string impactSoft2Sound = "Resources/Sounds/Cube/impact_soft2.wav";
		std::string impactSoft3Sound = "Resources/Sounds/Cube/impact_soft3.wav";
		std::string impactHard1Sound = "Resources/Sounds/Cube/impact_hard1.wav";
		std::string impactHard2Sound = "Resources/Sounds/Cube/impact_hard2.wav";
		std::string impactHard3Sound = "Resources/Sounds/Cube/impact_hard3.wav";

		m_pSounds[SoundLibrary::Grab] = soundManager->LoadSound(grabSound, false, true);
		m_pSounds[SoundLibrary::impactSoft1] = soundManager->LoadSound(impactSoft1Sound, false, true);
		m_pSounds[SoundLibrary::impactSoft2] = soundManager->LoadSound(impactSoft2Sound, false, true);
		m_pSounds[SoundLibrary::impactSoft3] = soundManager->LoadSound(impactSoft3Sound, false, true);
		m_pSounds[SoundLibrary::impactHard1] = soundManager->LoadSound(impactHard1Sound, false, true);
		m_pSounds[SoundLibrary::impactHard2] = soundManager->LoadSound(impactHard2Sound, false, true);
		m_pSounds[SoundLibrary::impactHard3] = soundManager->LoadSound(impactHard3Sound, false, true);
	}


	m_pRigidBody = m_pGameObject->GetComponent<RigidBodyComponent>();
	if (!m_pRigidBody)
	{
		Logger::LogError(L"PickUpObjectComponent::Initialize() > No RigidBodyComponent found on the GameObject");
		Logger::LogError(L"PickUpObjectComponent::Initialize() > Please add a RigidBodyComponent before adding this one");
	}
}


PickUpObjectComponent::SoundLibrary PickUpObjectComponent::GetRandomSound(impactType impactType)
{
	int random = rand() % 3 + 1;
	switch (impactType)
	{
		case PickUpObjectComponent::impactType::soft:
			return static_cast<SoundLibrary>(random);
		case PickUpObjectComponent::impactType::hard:
			random += 3;								 // just look at the soundlibrary, 3 soft and 3 hard sounds
			return static_cast<SoundLibrary>(random);
			default:
				return static_cast<SoundLibrary>(random);
	}
}

void PickUpObjectComponent::Update(const SceneContext& sceneContext)
{
	if (m_IsGrabbed && m_pRigidBody)
	{
		TransformComponent* pCameraTransform = sceneContext.pCamera->GetTransform();
		XMVECTOR wishPosV = XMLoadFloat3(&pCameraTransform->GetWorldPosition());

		XMVECTOR offsetFromCameraV = XMLoadFloat3(&pCameraTransform->GetForward());
		offsetFromCameraV = XMVectorScale(offsetFromCameraV, Character::GetMaxPickupDistance());

		XMFLOAT3 offsetFromCamera;
		XMStoreFloat3(&offsetFromCamera, offsetFromCameraV);

		wishPosV = XMVectorAdd(wishPosV, offsetFromCameraV);

		XMVECTOR currentPosV = XMLoadFloat3(&GetTransform()->GetWorldPosition());

		XMVECTOR cubeToWishPosV = XMVectorSubtract(wishPosV, currentPosV);

		const float THRESHOLD_DISTANCE = 3.f;
		float distance = XMVectorGetX(XMVector3Length(cubeToWishPosV));
		// Check if distance exceeds threshold and update m_IsGrabbed if necessary
		if (distance > THRESHOLD_DISTANCE)
		{
			m_IsGrabbed = false;
			return; // Exit the function since we no longer want to process the rest if it's not grabbed.
		}

		// new variable
		XMFLOAT4 cameraRotation = pCameraTransform->GetWorldRotation();
		// new variable

		// Calculate quaternion difference between current rotation and desired rotation
		XMFLOAT4 currentRotation = GetTransform()->GetWorldRotation();
		XMVECTOR currentRotationQ = XMLoadFloat4(&currentRotation);
		XMVECTOR desiredRotationQ = XMLoadFloat4(&cameraRotation);

		// Normalize the quaternions before calculations
		currentRotationQ = XMQuaternionNormalize(currentRotationQ);
		desiredRotationQ = XMQuaternionNormalize(desiredRotationQ);

		// Calculate quaternion difference between current rotation and desired rotation
		XMVECTOR rotationDifferenceQ = XMQuaternionMultiply(XMQuaternionInverse(currentRotationQ), desiredRotationQ);

		// Convert quaternion difference to axis and angle
		XMFLOAT3 rotationAxis;
		XMVECTOR rotationAxisV;
		float rotationAngle;
		XMQuaternionToAxisAngle(&rotationAxisV, &rotationAngle, rotationDifferenceQ);
		XMStoreFloat3(&rotationAxis, rotationAxisV);

		// If the rotation angle is very small, skip torque application
		if (rotationAngle > XMConvertToRadians(0.1f))
		{
			// Define a scaling factor to adjust the strength of the torque. This can be tweaked for better results.
			const float TORQUE_SCALING_FACTOR = 5.0f;

			// Apply torque based on axis and angle to align object with desired orientation
			XMFLOAT3 torque;
			torque.x = rotationAxis.x * rotationAngle * TORQUE_SCALING_FACTOR;
			torque.y = rotationAxis.y * rotationAngle * TORQUE_SCALING_FACTOR;
			torque.z = rotationAxis.z * rotationAngle * TORQUE_SCALING_FACTOR;
			//m_pRigidBody->AddTorque(torque);
		}


		XMFLOAT3 cubeToWishPos;
		XMStoreFloat3(&cubeToWishPos, cubeToWishPosV);

		XMFLOAT3 origin = GetTransform()->GetWorldPosition();
		XMFLOAT3 directionNorm;

		XMVECTOR directionVNorm = XMVector3Normalize(cubeToWishPosV);
		XMStoreFloat3(&directionNorm, directionVNorm);


		/*
		* Issue: When grabbing an object and pointing it at a wall, it jitters due to conflicts in grab physics.
		* Solution:
		* 1. Use raycasting to check if there's a wall in front of the object using the `cubeToWishPos` vector.
		* 2. If there's a wall, employ the `AddForce` method to move the object, which avoids the jitter.
		*
		* Note:
		* - Why not always use `AddForce`? It makes the object bounce uncontrollably when grabbing. Yet, it works well against walls.
		* - The raycast distance includes `+1.f` to account for scenarios where the grab point is near the wall.
		*   The object's origin is its center, and with a size of 1, not adding `+1.f` would cause jittering.
		*/
		PxQueryFilterData filterData{};
		filterData.data.word0 = ~static_cast<UINT>(CollisionGroup::Group0 | CollisionGroup::Group5);
		PxRaycastBuffer pxHit{};
		if (m_pScene->GetPhysxProxy()->Raycast(PxVec3(origin.x, origin.y, origin.z), PxVec3(directionNorm.x, directionNorm.y, directionNorm.z), distance + 1.f, pxHit, PxHitFlag::eDEFAULT, filterData))
			m_pRigidBody->AddForce(cubeToWishPos, physx::PxForceMode::eVELOCITY_CHANGE);
		else
		{
			cubeToWishPosV = XMVectorScale(cubeToWishPosV, 15.f);
			XMStoreFloat3(&cubeToWishPos, cubeToWishPosV);

			m_pRigidBody->SetLinearVelocity(cubeToWishPos);
		}
	}
}

void PickUpObjectComponent::OnContact(float impactForce)
{
	std::cout << "PickUpObjectComponent::OnContact() > Impact force: " << impactForce << std::endl;

	if (impactForce > m_ImpactThreshholdHard) 
	{
		// this math doesnt make sense and it shouldnt make sense, it is just a stupid way to get louder sounds with larger impact forces
		float volume = 0.2f + impactForce / (2 * m_ImpactThreshholdHard);
		volume = std::min(1.f, volume);
		SoundManager::Get()->Play3DSound(m_pSounds[GetRandomSound(impactType::hard)], volume, GetTransform()->GetWorldPosition(), SoundManager::SoundChannel::Level, true);
	}
	else if (impactForce > m_ImpactThreshholdSoft) 
	{
		// this math doesnt make sense and it shouldnt make sense, it is just a stupid way to get louder sounds with larger impact forces
		float volume = 0.2f + impactForce / (2 * m_ImpactThreshholdSoft);
		volume = std::min(1.f, volume);
		SoundManager::Get()->Play3DSound(m_pSounds[GetRandomSound(impactType::soft)], volume, GetTransform()->GetWorldPosition(), SoundManager::SoundChannel::Level, true);
	}
}
