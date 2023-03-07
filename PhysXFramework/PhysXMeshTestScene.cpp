#include "stdafx.h"
#include "PhysXMeshTestScene.h"
#include "PhysxManager.h"
#include "ContentManager.h"
#include "MeshObject.h"

void PhysXMeshTestScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = pPhysX->createMaterial(.5f, .5f, .2f);

	// Ground plane
	auto pGroundActor = pPhysX->createRigidStatic(PxTransform{ {PxPiDivTwo,{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, *pDefaultMaterial);
	m_pPhysxScene->addActor(*pGroundActor);

	// Convex chair
	m_pConvexChair = new MeshObject(L"Resources/Meshes/Chair.ovm");
	AddGameObject(m_pConvexChair);

	auto pConvexCollisionMesh = ContentManager::GetInstance()->Load<PxConvexMesh>(L"Resources/Meshes/Chair.ovpc");
	auto pConvexCollisionActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });

	PxRigidActorExt::createExclusiveShape(*pConvexCollisionActor, PxConvexMeshGeometry{ pConvexCollisionMesh }, *pDefaultMaterial);
	auto centerOfMass = PxVec3{ 0.f,5.f,0.f };
	PxRigidBodyExt::updateMassAndInertia(*pConvexCollisionActor, 10.f /*, &centerOfMass*/);


	m_pConvexChair->AttachRigidActor(pConvexCollisionActor);
	m_pConvexChair->Translate(0.f, 12.f, 0.f);
	m_pConvexChair->RotateDegrees(30.f, 0.f, 0.f);

	// Triangle Chair
	m_pTriangleChair = new MeshObject(L"Resources/Meshes/Chair.ovm");
	AddGameObject(m_pTriangleChair);

	const auto pTrinangleMesh = ContentManager::GetInstance()->Load<PxTriangleMesh>(L"Resources/Meshes/Chair.ovpt");

	//const auto pTriangleStatic = pPhysX->createRigidStatic(PxTransform{ PxIdentity });
	//PxRigidActorExt::createExclusiveShape(*pTriangleStatic, PxTriangleMeshGeometry{ pTrinangleMesh }, *pDefaultMaterial);
	//m_pTriangleChair->AttachRigidActor(pTriangleStatic);

	m_pTriangleActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });

	// change the dynamic actor into kinemtic 
	//(TRIANGLE COLLISION MESHES SHOULD ALWAYS BE KINEMATIC, THIS IS JUST DEMO ON HOW TO SET IT KINEMATIC IN CASE YOU FORGOT)
	m_pTriangleActor->setRigidBodyFlag( PxRigidBodyFlag::eKINEMATIC, true);

	PxRigidActorExt::createExclusiveShape(*m_pTriangleActor, PxTriangleMeshGeometry{ pTrinangleMesh }, *pDefaultMaterial);

	m_pTriangleChair->AttachRigidActor(m_pTriangleActor);

	m_pTriangleChair->Translate(10,0,0);

	m_KinematicPosition = { 10,0,0 };
}

void PhysXMeshTestScene::Update()
{
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_LEFT))
	{
		m_KinematicPosition.x -= 10.f * m_SceneContext.GetGameTime()->GetElapsed();
		m_pTriangleActor->setKinematicTarget(PxTransform{m_KinematicPosition });
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_RIGHT))
	{
		m_KinematicPosition.x += 10.f * m_SceneContext.GetGameTime()->GetElapsed();
		m_pTriangleActor->setKinematicTarget(PxTransform{ m_KinematicPosition });
	}
}

void PhysXMeshTestScene::Draw() const
{
}

void PhysXMeshTestScene::OnSceneActivated()
{
}

void PhysXMeshTestScene::OnSceneDeactivated()
{
}
