#include "stdafx.h"
#include "BouncingBallScene.h"

#include "SpherePosColorNorm.h"

#include "Logger.h"


void BouncingBallScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	InputAction reset = InputAction{ InputIds::reset, InputTriggerState::pressed, 0x52,-1,XINPUT_GAMEPAD_B };
	m_SceneContext.GetInput()->AddInputAction(reset);


	XMFLOAT3 actorDimensions{ 1.1f,1.f,1.f };
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	// Ball Actor
	for (size_t idx = 0; idx < m_NrOfSpheres; idx++)
	{

		m_pSpheres.push_back(new SpherePosColorNorm(1, 16, 16, XMFLOAT4{ Colors::Green }));
		AddGameObject(m_pSpheres.at(idx));

		// PhysX
		m_pSphereActors.push_back(pPhysX->createRigidDynamic(PxTransform(PxIdentity)));

		PxSphereGeometry sphereGeometry = PxSphereGeometry(1.f);
		// all the friction values
		float bounciness = 1.f;
		bounciness -= (float)idx / 2.f;
		PxMaterial* pSphereMaterial = pPhysX->createMaterial(0.0f, 0.0f, bounciness );

		//// bind the created varibles to the physics object
		//PxShape* pBoxShape = pPhysX->createShape(boxGeometry, *pBoxMaterial, true);
		//m_pCubeActor->attachShape(*pBoxShape);
		//m_pPhysxScene->addActor(*m_pCubeActor);

		PxRigidActorExt::createExclusiveShape(*m_pSphereActors.at(idx), sphereGeometry, &pSphereMaterial, 1);
		m_pSpheres.at(idx)->AttachRigidActor(m_pSphereActors.at(idx));

		XMFLOAT3 spawnLocation{ m_SphereSpawnLocation };

		spawnLocation.x += m_SphereSpawnOffset * idx;
		m_pSpheres.at(idx)->Translate(spawnLocation.x, spawnLocation.y,spawnLocation.z);
	}


	// Create a ground plane and add it to the physics scene
	PxRigidStatic* pGroundActor = pPhysX->createRigidStatic(PxTransform
	(
		PxQuat(PxPiDivTwo, PxVec3{ 0.f,0.f,1.f })
	));
	PxMaterial* pFloorMaterial = pPhysX->createMaterial(0.0f, 0.0f, 1.f);
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, &pFloorMaterial, 1);
	m_pPhysxScene->addActor(*pGroundActor);

	// Camera
	m_SceneContext.GetCamera()->SetPositionAndLookAt(XMFLOAT3{ 0.f,10.f,-20.f }, XMFLOAT3{ 0.f,3.5f,0.f });

}

void BouncingBallScene::Update()
{
	bool		resetInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::reset);
	if (resetInput)
	{
		OnSceneActivated();
	}
}

void BouncingBallScene::Draw() const
{
}

void BouncingBallScene::OnSceneActivated() 
{
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"Scene Activated > \"%ls\"", GetName().c_str());
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"\t[INPUT > Reset='R']");

	for (size_t idx = 0; idx < m_NrOfSpheres; idx++)
	{
		XMFLOAT3 spawnLocation{ m_SphereSpawnLocation };

		spawnLocation.x += m_SphereSpawnOffset * idx;
		m_pSpheres.at(idx)->Translate(spawnLocation.x, spawnLocation.y, spawnLocation.z);
	}
}

void BouncingBallScene::OnSceneDeactivated()
{
}
