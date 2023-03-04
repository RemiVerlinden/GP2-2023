#include "stdafx.h"
#include "W2_AssignmentScene.h"
#include "PhysxManager.h"
#include "ContentManager.h"
#include "MeshObject.h"
#include "SpherePosColorNorm.h"
#include "CubePosColorNorm.h"
#include "SoundManager.h"

void W2_AssignmentScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = pPhysX->createMaterial(1.f, 1.f, .0f);

	// Ground plane
	auto pGroundActor = pPhysX->createRigidStatic(PxTransform{ {PxPiDivTwo,{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, *pDefaultMaterial);
	m_pPhysxScene->addActor(*pGroundActor);

	// Triangle Level 
	m_pTriangleLevel = new MeshObject(L"Resources/Meshes/Level.ovm");
	AddGameObject(m_pTriangleLevel);

	const auto pTrinangleMesh = ContentManager::GetInstance()->Load<PxTriangleMesh>(L"Resources/Meshes/Level.ovpt");

	m_pTriangleActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });

	// change the dynamic actor into kinemtic 
	//(TRIANGLE COLLISION MESHES SHOULD ALWAYS BE KINEMATIC, THIS IS JUST DEMO ON HOW TO SET IT KINEMATIC IN CASE YOU FORGOT)
	m_pTriangleActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	PxRigidActorExt::createExclusiveShape(*m_pTriangleActor, PxTriangleMeshGeometry{ pTrinangleMesh }, *pDefaultMaterial);

	m_pTriangleLevel->AttachRigidActor(m_pTriangleActor);

	// Sphere
	{
		m_pSphere = new SpherePosColorNorm(1, 10, 10, XMFLOAT4{ Colors::Gray });
		AddGameObject(m_pSphere);

		auto pSphereActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
		PxRigidActorExt::createExclusiveShape(*pSphereActor, PxSphereGeometry{ 1.f }, *pDefaultMaterial);

		m_pSphere->AttachRigidActor(pSphereActor);

		pSphereActor->setMass(m_SphereMass);

		XMFLOAT3 pos{ m_SpherePos };
		m_pSphere->Translate(pos.x, pos.y, pos.z); \

		{
			// Lock Sphere's Z-Axis

			LockAxisZ(pSphereActor);
		}
	}

	// Dynamic Boxes
	XMFLOAT3 cubeMaterial{ 0.f, 0.f, .2f };
	{
		GameObject* pCube = nullptr;
		PxRigidDynamic* pCubeActor = nullptr;

		XMFLOAT3 cubeTransform{ m_DynamicCubeSpawnPos };

		// blue left cube
		CreatePxDynamicCube(pCube, pCubeActor, cubeTransform, { 0,0,0 }, { 1.5f,1.5f,1.5f }, cubeMaterial, XMFLOAT4{ Colors::Blue });
		m_pDynamicCubeVec.push_back(pCube);
		LockAxisZ(pCubeActor);

		// red right cube
		cubeTransform.x += 8;
		CreatePxDynamicCube(pCube, pCubeActor, cubeTransform, { 0,0,0 }, { 1.5f,1.5f,1.5f }, cubeMaterial, XMFLOAT4{ Colors::Red });
		m_pDynamicCubeVec.push_back(pCube);
		LockAxisZ(pCubeActor);
	}

	// triggers with color

	// left trigger
	{
		XMFLOAT3 pos{ -7.3f, 2.4f, 0 };
		XMFLOAT3 size{ 2.5f, .6f, 6.f };
		//mesh with color and no collision
		auto* pBoxMesh = new CubePosColorNorm(2.5f, .6f, 6.f, XMFLOAT4{ Colors::DarkBlue });
		pBoxMesh->Translate(pos.x, pos.y, pos.z);
		AddGameObject(pBoxMesh);

		m_pTriggerLeft = pPhysX->createRigidStatic(PxTransform{ pos.x,pos.y,pos.z });
		auto pShape = PxRigidActorExt::createExclusiveShape(*m_pTriggerLeft, PxBoxGeometry{ size.x / 2,size.y / 2,size.z / 2 }, *pDefaultMaterial);

		pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		m_pPhysxScene->addActor(*m_pTriggerLeft);
	}
	// right trigger
	{
		XMFLOAT3 pos{ 6.6f, 2.4f, 0 };
		XMFLOAT3 size{ 2.5f, .6f, 6.f };
		//mesh with color and no collision
		auto* pBoxMesh = new CubePosColorNorm(2.5f, .6f, 6.f, XMFLOAT4{ Colors::DarkRed });
		pBoxMesh->Translate(pos.x, pos.y, pos.z);
		AddGameObject(pBoxMesh);

		m_pTriggerRight = pPhysX->createRigidStatic(PxTransform{ pos.x,pos.y,pos.z });
		auto pShape = PxRigidActorExt::createExclusiveShape(*m_pTriggerRight, PxBoxGeometry{ size.x / 2,size.y / 2,size.z / 2 }, *pDefaultMaterial);

		pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		m_pPhysxScene->addActor(*m_pTriggerRight);
	}

	// Create Traps doors
	{
		XMFLOAT3 pos{ 8.9f, 17.05f, 0 };
		XMFLOAT3 size{ 2.35f,0.5f,3.f };
		//mesh with color and no collision
		auto* pBoxMesh = new CubePosColorNorm(size.x, size.y, size.z, XMFLOAT4{ Colors::Purple });
		pBoxMesh->Translate(0, 0, 0);
		//pBoxMesh->Translate(pos.x, pos.y, pos.z);
		AddGameObject(pBoxMesh);
		temp = pBoxMesh;

		GameObject* pCube = nullptr;
		PxRigidDynamic* pCubeActor = nullptr;

		XMFLOAT3 cubeTransform{ m_TrapDoorSpawnPos1 };

		// blue left trapdoor
		{
			CreatePxDynamicCube(pCube, pCubeActor, cubeTransform, { 0,0,0 }, { size.x, size.y, size.z }, cubeMaterial, XMFLOAT4{ Colors::Blue });
			m_pTrapdoorVec.push_back(pCube);

			auto levelActor = m_pTriangleLevel->GetRigidActor()->is<PxRigidDynamic>();
			m_pLeftfJoint = PxRevoluteJointCreate(*pPhysX, pCubeActor,
				PxTransform{ size.x / 2,-size.y / 2,0, PxQuat{-PxPi / 2, PxVec3{0,-1,0}} },
				NULL,
				PxTransform{ cubeTransform.x + size.x / 2,cubeTransform.y - size.y / 2,0, PxQuat{PxPi / 2, PxVec3{0,1,0}} });

			m_pLeftfJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
			m_pLeftfJoint->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
			m_pLeftfJoint->setDriveVelocity(-.5f);
		}
		// red right trapdoor
		{
			cubeTransform = m_TrapDoorSpawnPos2;
			CreatePxDynamicCube(pCube, pCubeActor, cubeTransform, { 0,0,0 }, { size.x, size.y, size.z }, cubeMaterial, XMFLOAT4{ Colors::Red });
			m_pTrapdoorVec.push_back(pCube);

			auto levelActor = m_pTriangleLevel->GetRigidActor()->is<PxRigidDynamic>();
			m_pRightJoint = PxRevoluteJointCreate(*pPhysX, pCubeActor,
				PxTransform{ -size.x / 2,-size.y / 2,0, PxQuat{-PxPi / 2, PxVec3{0,-1,0}} },
				NULL,
				PxTransform{ cubeTransform.x - size.x / 2,cubeTransform.y - size.y / 2,0, PxQuat{PxPi / 2, PxVec3{0,1,0}} });

			m_pRightJoint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
			m_pRightJoint->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
			m_pRightJoint->setDriveVelocity(.5f);
		}
	}


	// spheres on top of trap doors
	{
		XMFLOAT3 pos{ m_TrapDoorSphereSpawnPos };
		GameObject* pGo;
		PxRigidDynamic* pActor;
		CreatePxDynamicSphere(pGo, pActor, pos, { 0,0,0 }, 1.f, cubeMaterial, XMFLOAT4{ Colors::Gray });
		m_pDynamicSphereVec.push_back(pGo);
		LockAxisZ(pActor);

		pos.x += 19;
		CreatePxDynamicSphere(pGo, pActor, pos, { 0,0,0 }, 1.f, cubeMaterial, XMFLOAT4{ Colors::Gray });
		m_pDynamicSphereVec.push_back(pGo);
		LockAxisZ(pActor);

	}


	// FMOD SOUND 
	{
		auto pFmod = SoundManager::GetInstance()->GetSystem();

		FMOD_RESULT result = pFmod->createStream("Resources/Sounds/bell_blue_oneshot.wav", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pLeftBell);
		SoundManager::GetInstance()->ErrorCheck(result);

		 result = pFmod->playSound(m_pLeftBell, nullptr, true, &m_pChannel2D);
		SoundManager::GetInstance()->ErrorCheck(result);

		result = pFmod->createStream("Resources/Sounds/bell_red_oneshot.wav", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pRightBell);
		SoundManager::GetInstance()->ErrorCheck(result);

		 result = pFmod->playSound(m_pRightBell, nullptr, true, &m_pChannel2D);
		SoundManager::GetInstance()->ErrorCheck(result);
	}
}


void W2_AssignmentScene::Update()
{
	float timeStep = m_SceneContext.GetGameTime()->GetElapsed();
	float force = m_SphereForce * timeStep;
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_LEFT))
	{
		// only rigidDynamic can have torque applied to we have to tell it specifically that we are
		// dealing with a rigid DYNAMIC actor
		m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->addTorque({ 0,0,force });
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_RIGHT))
	{
		// only rigidDynamic can have torque applied to we have to tell it specifically that we are
		// dealing with a rigid DYNAMIC actor
		m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->addTorque({ 0,0,-force });
	}

	// JUMP INPUT
	{
		//calculate forces
		PxVec3 jumpforce{ 0,0,0 };

		//normalize horizontal movement
		jumpforce;
		jumpforce *= m_SphereJumpForce * timeStep;

		if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, VK_SPACE))
		{
			jumpforce.y += m_SphereJumpForce * 100.f * timeStep;
		}

		m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->addForce(jumpforce);
	}


	if (m_EnableLeftJoint)
	{
		m_pLeftfJoint->setDriveVelocity(3.f);
		m_EnableLeftJoint = false;

		auto pFmod = SoundManager::GetInstance()->GetSystem();
		FMOD_RESULT result = pFmod->playSound(m_pLeftBell, nullptr, false, &m_pChannel2D);
		SoundManager::GetInstance()->ErrorCheck(result);
		m_pChannel2D->setVolume(0.5f);
	}
	if (m_EnableRightJoint)
	{
		m_pRightJoint->setDriveVelocity(-3.f);
		m_EnableRightJoint = false;

		auto pFmod = SoundManager::GetInstance()->GetSystem();
		FMOD_RESULT result = pFmod->playSound(m_pRightBell, nullptr, false, &m_pChannel2D);
		SoundManager::GetInstance()->ErrorCheck(result);
		m_pChannel2D->setVolume(0.5f);
	}


	//if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, 'V'))
	//{
	//	float volume{};
	//	m_pChannel2D->getVolume(&volume);
	//	volume += 0.1f;
	//	Clamp(volume, 1.f, 0.f);
	//	m_pChannel2D->setVolume(volume);
	//	Logger::GetInstance()->LogFormat(LogLevel::Info, L"Sound Volume Changed > %f", volume);
	//}
	//if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::pressed, 'C'))
	//{
	//	float volume{};
	//	m_pChannel2D->getVolume(&volume);
	//	volume -= 0.1f;
	//	Clamp(volume, 1.f, 0.f);
	//	m_pChannel2D->setVolume(volume);
	//	Logger::GetInstance()->LogFormat(LogLevel::Info, L"Sound Volume Changed > %f", volume);
	//}

	//XMFLOAT3 trans = temp->GetPosition();
	//if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_LEFT))
	//{
	//	trans.x -= 10.f * timeStep;
	//	temp->Translate(trans.x,trans.y,trans.z);
	//}
	//if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_RIGHT))
	//{
	//	trans.x += 10.f * timeStep;

	//	temp->Translate(trans.x,trans.y,trans.z);
	//}
	//if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_UP))
	//{
	//	trans.y += 10.f * timeStep;

	//	temp->Translate(trans.x,trans.y,trans.z);
	//}
	//if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_DOWN))
	//{
	//	trans.y -= 10.f * timeStep;

	//	temp->Translate(trans.x,trans.y,trans.z);
	//}

	//Logger::GetInstance()->LogDebug(std::to_wstring(trans.x) + L", " + std::to_wstring(trans.y));
}

void W2_AssignmentScene::Draw() const
{
}

void W2_AssignmentScene::OnSceneActivated()
{
	int idx{ 0 };
	for (auto pSphere : m_pDynamicSphereVec)
	{
		XMFLOAT3 pos{ m_TrapDoorSphereSpawnPos };
		pos.x += idx * 19.f;
		pSphere->Rotate(0, 0, 0);
		pSphere->Translate(pos.x, pos.y, pos.z);
		++idx;
	}

	idx = 0;
	for (auto pCube : m_pDynamicCubeVec)
	{
		XMFLOAT3 pos{ m_DynamicCubeSpawnPos };

		pos.x += idx * 8.f;
		pCube->Rotate(0, 0, 0);
		pCube->Translate(pos.x, pos.y, pos.z);
		++idx;
	}

	{
		XMFLOAT3 pos{ m_SpherePos };
		m_pSphere->Rotate(0, 0, 0);
		m_pSphere->Translate(pos.x, pos.y, pos.z);
	}
	{
		for (auto trapdoor : m_pTrapdoorVec)
		{
			trapdoor->Rotate(0, 0, 0);
		}
		m_pLeftfJoint->setDriveVelocity(-0.5f);

		m_pRightJoint->setDriveVelocity(0.5f);
	}
}

void W2_AssignmentScene::OnSceneDeactivated()
{
}

void W2_AssignmentScene::CreatePxDynamicCube(GameObject*& cube, PxRigidDynamic*& cubeActor,
	XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material, XMFLOAT4 color
)
{
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	cube = new CubePosColorNorm(size.x, size.y, size.z, color);
	AddGameObject(cube);

	// PhysX
	cubeActor = pPhysX->createRigidDynamic(PxTransform(PxIdentity));

	PxMaterial* pBoxMaterial = pPhysX->createMaterial(material.x, material.y, material.z);

	PxBoxGeometry boxGeometry = PxBoxGeometry
	(
		PxVec3
		{
			size.x / 2.f,
			size.y / 2.f,
			size.z / 2.f
		}
	);

	PxRigidActorExt::createExclusiveShape(*cubeActor, boxGeometry, &pBoxMaterial, 1);
	cube->AttachRigidActor(cubeActor);

	cube->Translate(pos.x, pos.y, pos.z);
	cube->RotateDegrees(rotation.x, rotation.y, rotation.z);
}

void W2_AssignmentScene::CreatePxDynamicSphere(GameObject*& sphere, PxRigidDynamic*& sphereActor,
	XMFLOAT3 pos, XMFLOAT3 rotation, float radius, XMFLOAT3 material, XMFLOAT4 color
)
{
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	sphere = new SpherePosColorNorm(radius, 16, 16, color);
	AddGameObject(sphere);

	// PhysX
	sphereActor = pPhysX->createRigidDynamic(PxTransform(PxIdentity));

	PxMaterial* pSphereMaterial = pPhysX->createMaterial(material.x, material.y, material.z);

	PxSphereGeometry sphereGeometry = PxSphereGeometry(radius);

	PxRigidActorExt::createExclusiveShape(*sphereActor, sphereGeometry, &pSphereMaterial, 1);
	sphere->AttachRigidActor(sphereActor);

	sphere->Translate(pos.x, pos.y, pos.z);
	sphere->RotateDegrees(rotation.x, rotation.y, rotation.z);
}

void W2_AssignmentScene::LockAxisZ(PxRigidDynamic* actor)
{
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	// Lock Sphere's Z-Axis
	auto d6joint = PxD6JointCreate(*pPhysX, nullptr, PxTransform{ PxIdentity }, actor, PxTransform{ PxIdentity });
	d6joint->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
	d6joint->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
	d6joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
	d6joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	d6joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
}

void W2_AssignmentScene::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; ++i)
	{
		// ignore if deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_OTHER | PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER))
			continue;

		const PxTriggerPair pair = pairs[i];
		if (pair.triggerActor == m_pTriggerLeft)
		{
			if (pair.otherActor == m_pDynamicCubeVec.at(0)->GetRigidActor()->is<PxRigidDynamic>())
			{
				if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND) // ENTER
				{
					m_EnableLeftJoint = true;
				}
			}
		}

		if (pair.triggerActor == m_pTriggerRight)
		{
			if (pair.otherActor == m_pDynamicCubeVec.at(1)->GetRigidActor()->is<PxRigidDynamic>())
			{
				if (pair.status == PxPairFlag::eNOTIFY_TOUCH_FOUND) // ENTER
				{
					m_EnableRightJoint = true;
				}
			}
		}
	}
}