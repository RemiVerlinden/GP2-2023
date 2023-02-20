#include "stdafx.h"
#include "PhysXTestScene.h"

#include "CubePosColorNorm.h"

#include "Logger.h"


void PhysXTestScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	// Cube Actor
	XMFLOAT3 actorDimensions{ 1.5f,1.f,1.f };
	m_pCube = new CubePosColorNorm(actorDimensions.x,actorDimensions.y,actorDimensions.z);
	AddGameObject(m_pCube);

	// PhysX
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();
	m_pCubeActor = pPhysX->createRigidDynamic(PxTransform(PxIdentity));

	PxBoxGeometry boxGeometry = PxBoxGeometry
	(
		PxVec3
		{
			actorDimensions.x / 2.f,
			actorDimensions.y / 2.f,
			actorDimensions.z / 2.f
		}
	);
	// all the friction values
	PxMaterial* pBoxMaterial = pPhysX->createMaterial(0.5f,0.5f,1.f);

	//// bind the created varibles to the physics object
	//PxShape* pBoxShape = pPhysX->createShape(boxGeometry, *pBoxMaterial, true);
	//m_pCubeActor->attachShape(*pBoxShape);
	//m_pPhysxScene->addActor(*m_pCubeActor);

	PxRigidActorExt::createExclusiveShape(*m_pCubeActor, boxGeometry, &pBoxMaterial, 1);
	m_pCube->AttachRigidActor(m_pCubeActor);

	m_pCube->Translate(0.f, 5.f, 0.f);

	// Create a ground plane and add it to the physics scene
	PxRigidStatic* pGroundActor = pPhysX->createRigidStatic(PxTransform
									(
										PxQuat(PxPiDivTwo, PxVec3{0.f,0.f,1.f})
									));
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, &pBoxMaterial, 1);
	m_pPhysxScene->addActor(*pGroundActor);

	// Camera
	m_SceneContext.GetCamera()->SetPositionAndLookAt(XMFLOAT3{ 0.f,5.f,-20.f }, XMFLOAT3{});



}

void PhysXTestScene::Update()
{
}

void PhysXTestScene::Draw() const
{
}

void PhysXTestScene::OnSceneActivated()
{
}

void PhysXTestScene::OnSceneDeactivated()
{
}
