#include "stdafx.h"
#include "W1_AssignmentScene.h"

#include "CubePosColorNorm.h"
#include "SpherePosColorNorm.h"
#include "TorusPosColorNorm.h"
#include "CylinderPosColorNorm.h"

#include "Logger.h"

void W1_AssignmentScene::Initialize()
{
	EnablePhysxDebugRendering(true);



	// Camera
	m_SceneContext.GetCamera()->SetPositionAndLookAt(XMFLOAT3{ -10.f,6.f,-10.f }, XMFLOAT3{ 0.f,0.f,m_CubeWallSpawnDistance });

	// Input
	InputAction reset = InputAction{ InputIds::reset, InputTriggerState::pressed, 0x52,-1,XINPUT_GAMEPAD_B };
	m_SceneContext.GetInput()->AddInputAction(reset);

	InputAction moveLeft = InputAction{ InputIds::moveLeft, InputTriggerState::down, VK_LEFT,-1,XINPUT_GAMEPAD_DPAD_LEFT };
	InputAction moveRight = InputAction{ InputIds::moveRight, InputTriggerState::down, VK_RIGHT,-1,XINPUT_GAMEPAD_DPAD_RIGHT };
	InputAction moveUp = InputAction{ InputIds::moveUp, InputTriggerState::down, VK_UP,-1,XINPUT_GAMEPAD_DPAD_UP };
	InputAction moveDown = InputAction{ InputIds::moveDown, InputTriggerState::down, VK_DOWN,-1,XINPUT_GAMEPAD_DPAD_DOWN };
	InputAction jump = InputAction{ InputIds::jump, InputTriggerState::pressed, VK_SPACE,-1,XINPUT_GAMEPAD_A };
	m_SceneContext.GetInput()->AddInputAction(moveLeft);
	m_SceneContext.GetInput()->AddInputAction(moveRight);
	m_SceneContext.GetInput()->AddInputAction(moveUp);
	m_SceneContext.GetInput()->AddInputAction(moveDown);
	m_SceneContext.GetInput()->AddInputAction(jump);

	// cubes
	int totalCubes = m_TotalHorizontalCubes * m_TotalVerticalCubes;
	for (size_t yCube = 0; yCube < m_TotalVerticalCubes; ++yCube)
	{
		for (size_t xCube = 0; xCube < m_TotalHorizontalCubes; ++xCube)
		{

			// just calculating the position for each cube
			XMFLOAT3 cubeStartPos{ -m_TotalHorizontalCubes / 2.f * m_CubeDimentions.x ,0,m_CubeWallSpawnDistance }; // just make sure that the cube stack spawns in the center

			cubeStartPos.x += m_CubeDimentions.x / 2;
			cubeStartPos.y += m_CubeDimentions.y / 2;

			cubeStartPos.x += xCube * (m_CubeDimentions.x + m_CubeWiggleRoom);
			cubeStartPos.y += yCube * (m_CubeDimentions.y);


			XMFLOAT3 randomRotation{ 0,0,0 };
			randomRotation.y = -m_MaxRandomSpawnRotation + +rand() % m_MaxRandomSpawnRotation * 2;

			GameObject* cube = nullptr;
			PxRigidDynamic* cubeActor = nullptr;

			// create the physics cubes
			CreatePxDynamicCube(cube, cubeActor, cubeStartPos, randomRotation, m_CubeDimentions, m_CubeMatValues);
			m_pCubeVec.push_back(cube);
			m_pCubeActorVec.push_back(cubeActor);
		}
	}

	// create the sphere
	CreatePxDynamicSphere(m_pSphere, m_pSphereActor, XMFLOAT3{ 0,m_SphereRadius,0 }, XMFLOAT3{}, m_SphereRadius, m_SphereMatValues);
	m_pSphereActor->setMass(m_SphereMass);

	// add a floor cube
	XMFLOAT3 floorSpawnPos{ 0,-m_FloorCubeSize.y/2,m_FloorCubeSize.z / 3 };
	GameObject* pFloor;
	PxRigidStatic* pFloorActor;
	CreatePxStaticCube(pFloor, pFloorActor, floorSpawnPos, XMFLOAT3{},m_FloorCubeSize, m_GroundMatValues, m_FloorColor);
}

void W1_AssignmentScene::Update()
{
	float gameTime = GetSceneContext().GetGameTime()->GetTotal();

	// Input
	bool		moveLeftInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveLeft);
	bool		moveRightInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveRight);
	bool		moveUpInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveUp);
	bool		moveDownInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveDown);
	bool		jumpInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::jump);

	bool		resetInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::reset);
	if (resetInput)
	{
		OnSceneActivated();
	}

	//calculate forces
	float		timeStep{ m_SceneContext.GetGameTime()->GetElapsed() };
	XMVECTOR ForceXMVEC{ 0,0,0 };
	const SceneContext& sceneContext = GetSceneContext();
	const BaseCamera* camera = sceneContext.GetCamera();
	if (moveLeftInput)
	{
		XMVECTOR forward = XMVectorAdd(ForceXMVEC, XMLoadFloat3(&camera->GetForward()));
		forward = XMVectorMultiply(forward, XMVECTOR{1,0,1});
		ForceXMVEC += forward;
	}

	if (moveRightInput)
	{
		XMVECTOR forward = XMVectorAdd(ForceXMVEC, XMLoadFloat3(&camera->GetForward()));
		forward = XMVectorMultiply(forward, XMVECTOR{ 1,0,1 });
		ForceXMVEC -= forward;
	}

	if (moveUpInput)
	{
		ForceXMVEC += XMVectorAdd(ForceXMVEC, XMLoadFloat3(&camera->GetRight()));
	}

	if (moveDownInput)
	{
		ForceXMVEC -= XMVectorAdd(ForceXMVEC, XMLoadFloat3(&camera->GetRight()));
	}

	//normalize horizontal movement
	XMFLOAT3 force;
	XMStoreFloat3(&force, ForceXMVEC);

	PxVec3 forceVec{ force.x,force.y,force.z };
	forceVec = forceVec.getNormalized();

	forceVec *= m_SphereTorqueForce * timeStep;

	m_pSphereActor->addTorque(forceVec);


	//calculate forces
	PxVec3 jumpforce{ 0,0,0 };


	//normalize horizontal movement
	jumpforce = jumpforce.getNormalized();
	jumpforce *= m_SphereJumpForce * timeStep;

	if (jumpInput)
	{
		jumpforce.y += m_SphereJumpForce * 100.f * timeStep;
	}

	m_pSphereActor->addForce(jumpforce);
}


void W1_AssignmentScene::Draw() const
{

}

void W1_AssignmentScene::OnSceneActivated()
{
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"Scene Activated > \"%ls\"", GetName().c_str());
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"\t[INPUT > Reset='R' | Apply_Torque='Arrow Keys' | Apply_Upwards_Force='SPACE']");
	
	int totalCubes = m_TotalHorizontalCubes * m_TotalVerticalCubes;
	for (size_t yCube = 0; yCube < m_TotalVerticalCubes; ++yCube)
	{
		for (size_t xCube = 0; xCube < m_TotalHorizontalCubes; ++xCube)
		{
			int cubeIdx = yCube * m_TotalHorizontalCubes + xCube;
			GameObject* cube = m_pCubeVec.at(cubeIdx);

			// just calculating the position for each cube

			XMFLOAT3 cubeStartPos{ -m_TotalHorizontalCubes / 2.f * m_CubeDimentions.x ,0,m_CubeWallSpawnDistance }; // just make sure that the cube stack spawns in the center


			cubeStartPos.x += m_CubeDimentions.x / 2;
			cubeStartPos.y += m_CubeDimentions.y / 2;

			cubeStartPos.x += xCube * (m_CubeDimentions.x + m_CubeWiggleRoom);
			cubeStartPos.y += yCube * (m_CubeDimentions.y);

			cube->Translate(cubeStartPos.x, cubeStartPos.y, cubeStartPos.z);
			int randomRotation = -m_MaxRandomSpawnRotation + +rand() % m_MaxRandomSpawnRotation * 2;
			cube->RotateDegrees(0, randomRotation, 0);
		}
	}

	m_pSphere->Translate(0,m_SphereRadius,0);
	m_pSphere->Rotate(0,0,0);
	m_pSphereActor->setForceAndTorque(PxVec3{ 0,0,0 }, PxVec3{ 0,0,0 });
}

void W1_AssignmentScene::OnSceneDeactivated()
{
}

void W1_AssignmentScene::CreatePxDynamicCube(GameObject*& cube, PxRigidDynamic*& cubeActor,
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

void W1_AssignmentScene::CreatePxStaticCube(GameObject*& cube, PxRigidStatic*& cubeActor,
	XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material
)
{
	CreatePxStaticCube(cube, cubeActor, pos, rotation, size, material, XMFLOAT4{0,0,0,0});
}

void W1_AssignmentScene::CreatePxStaticCube(GameObject*& cube, PxRigidStatic*& cubeActor,
	XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material, XMFLOAT4 color
)
{
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	if (color.w == 0.f)
	{
		cube = new CubePosColorNorm(size.x, size.y, size.z);
	}
	else
	{
		cube = new CubePosColorNorm(size.x, size.y, size.z, color);
	}
	AddGameObject(cube);

	// PhysX
	cubeActor = pPhysX->createRigidStatic(PxTransform(PxIdentity));

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

void W1_AssignmentScene::CreatePxDynamicSphere(GameObject*& sphere, PxRigidDynamic*& sphereActor,
	XMFLOAT3 pos, XMFLOAT3 rotation, float radius, XMFLOAT3 material
)
{
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	sphere = new SpherePosColorNorm(radius, 16, 16, XMFLOAT4{ Colors::Yellow });
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