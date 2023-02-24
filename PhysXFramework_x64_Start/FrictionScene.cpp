#include "stdafx.h"
#include "FrictionScene.h"

#include "CubePosColorNorm.h"

#include "Logger.h"


void FrictionScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	// input 
	InputAction reset = InputAction{ InputIds::reset, InputTriggerState::pressed, 0x52,-1,XINPUT_GAMEPAD_B };
	m_SceneContext.GetInput()->AddInputAction(reset);


	// Cube Actor
	XMFLOAT3 dynamcActorDimensions{ 1.f,1.f,1.f };
	XMFLOAT3 staticActorDimensions{ 5.f,1.f,1.f };
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();


	for (size_t idx = 0; idx < m_NrOfFrictionCubes; idx++)
	{
		// all the friction values
		float friction = 0.1f + idx * 0.1f; // 0 , 0.5 , 1
		XMFLOAT3 material{ friction * 2, friction, 0.f };

		XMFLOAT3 translate = m_CubeDefaultTranslate;
		translate.x += idx * m_CubeSpawnOffset;

		GameObject* cube = nullptr;
		PxRigidDynamic* dynamicCubeActor = nullptr;
		PxRigidStatic* staticCubeActor = nullptr;

		CreatePxDynamicCube(cube, dynamicCubeActor, translate, m_CubeDefaultRotation, dynamcActorDimensions, material);
		m_pDynamicCubeVec.push_back(cube);
		m_pCubeDynamicActorVec.push_back(dynamicCubeActor);

		translate = m_CubeDefaultTranslate;
		translate.x += idx * m_CubeSpawnOffset;
		translate.y -= m_PlatformVerticalSpawnOffset;

		CreatePxStaticCube(cube, staticCubeActor, translate, m_CubeDefaultRotation, staticActorDimensions, material);
		m_pStaticCubeVec.push_back(cube);
		m_pCubeStaticActorVec.push_back(staticCubeActor);
	}

	// Create a ground plane and add it to the physics scene
	PxRigidStatic* pGroundActor = pPhysX->createRigidStatic(PxTransform
	(
		PxQuat(PxPiDivTwo, PxVec3{ 0.f,0.f,1.f })
	));

	PxMaterial* pFloorMaterial = pPhysX->createMaterial(0.f, 0.f, 0.f);
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, &pFloorMaterial, 1);
	m_pPhysxScene->addActor(*pGroundActor);

	// Camera
	m_SceneContext.GetCamera()->SetPositionAndLookAt(XMFLOAT3{ 0.f,10.f,-25.f }, XMFLOAT3{0,3,0});



}

void FrictionScene::Update()
{
	bool		resetInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::reset);
	if (resetInput)
	{
		OnSceneActivated();
	}
}

void FrictionScene::Draw() const
{
}

void FrictionScene::OnSceneActivated()
{
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"Scene Activated > \"%ls\"", GetName().c_str());
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"\t[INPUT > Reset='R']");

	for (size_t idx = 0; idx < m_NrOfFrictionCubes; idx++)
	{
		XMFLOAT3 translate = m_CubeDefaultTranslate;
		translate.x += idx * m_CubeSpawnOffset;
		m_pDynamicCubeVec.at(idx)->Translate(translate.x, translate.y, translate.z);
		m_pDynamicCubeVec.at(idx)->RotateDegrees(m_CubeDefaultRotation.x, m_CubeDefaultRotation.y, m_CubeDefaultRotation.z);
	}
}

void FrictionScene::OnSceneDeactivated()
{
}

void FrictionScene::CreatePxStaticCube(GameObject*& cube, PxRigidStatic*& cubeActor,
										XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material
)
{
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	cube = new CubePosColorNorm(size.x, size.y, size.z);
	AddGameObject(cube);

	// PhysX
	cubeActor = pPhysX->createRigidStatic(PxTransform(PxIdentity));

	PxMaterial* pBoxMaterial = pPhysX->createMaterial(material.x,material.y,material.z);

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


void FrictionScene::CreatePxDynamicCube(GameObject*& cube, PxRigidDynamic*& cubeActor,
	XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material
)
{
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	cube = new CubePosColorNorm(size.x, size.y, size.z);
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
