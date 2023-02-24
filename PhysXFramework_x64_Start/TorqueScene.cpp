#include "stdafx.h"
#include "TorqueScene.h"

#include "CubePosColorNorm.h"
#include "TorusPosColorNorm.h"
#include "CylinderPosColorNorm.h"

#include "Logger.h"

void TorqueScene::Initialize()
{
	EnablePhysxDebugRendering(true);



	// Camera
	m_SceneContext.GetCamera()->SetPositionAndLookAt(XMFLOAT3{ 0.f,30.f,-20.f }, XMFLOAT3{ 0.f,0.f,-1.f });

	// Input
	InputAction reset = InputAction{ InputIds::reset, InputTriggerState::pressed, 0x52,-1,XINPUT_GAMEPAD_B };
	m_SceneContext.GetInput()->AddInputAction(reset);

	InputAction moveLeft = InputAction{ InputIds::moveLeft, InputTriggerState::down, VK_LEFT,-1,XINPUT_GAMEPAD_DPAD_LEFT };
	InputAction moveRight = InputAction{ InputIds::moveRight, InputTriggerState::down, VK_RIGHT,-1,XINPUT_GAMEPAD_DPAD_RIGHT };
	InputAction moveUp = InputAction{ InputIds::moveUp, InputTriggerState::down, VK_UP,-1,XINPUT_GAMEPAD_DPAD_UP };
	InputAction moveDown = InputAction{ InputIds::moveDown, InputTriggerState::down, VK_DOWN,-1,XINPUT_GAMEPAD_DPAD_DOWN };
	m_SceneContext.GetInput()->AddInputAction(moveLeft);
	m_SceneContext.GetInput()->AddInputAction(moveRight);
	m_SceneContext.GetInput()->AddInputAction(moveUp);
	m_SceneContext.GetInput()->AddInputAction(moveDown);


	// cube
	m_pCube = new CubePosColorNorm(m_CubeDimentions.x, m_CubeDimentions.y, m_CubeDimentions.z);
	AddGameObject(m_pCube);

	XMFLOAT3 cubeStartPos{ 0,m_CubeDimentions.y / 2.f + 1.f,0 }; // always start 1 unit in the air
	m_pCube->Translate(cubeStartPos.x, cubeStartPos.y, cubeStartPos.z);

	// actual physics for the cube
	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();

	m_pCubeActor = pPhysX->createRigidDynamic(PxTransform(PxIdentity)); // kinda like rigidbody component from unity

	PxMaterial* pMaterial = pPhysX->createMaterial(m_MaterialValues.x, m_MaterialValues.y, m_MaterialValues.z);

	PxBoxGeometry boxGeometry = PxBoxGeometry
	(
		PxVec3
		{
			m_CubeDimentions.x / 2.f,
			m_CubeDimentions.y / 2.f,
			m_CubeDimentions.z / 2.f
		}
	);

	PxRigidActorExt::createExclusiveShape(*m_pCubeActor, boxGeometry, &pMaterial, 1);
	m_pCube->AttachRigidActor(m_pCubeActor);

	// Create a ground plane and add it to the physics scene

	PxRigidStatic* pGroundActor = pPhysX->createRigidStatic(PxTransform
	(
		PxQuat(PxPiDivTwo, PxVec3{ 0.f,0.f,1.f })
	));

	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, &pMaterial, 1);
	m_pPhysxScene->addActor(*pGroundActor);


}

void TorqueScene::Update()
{
	float gameTime = GetSceneContext().GetGameTime()->GetTotal();

	// Input
	bool		moveLeftInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveLeft);
	bool		moveRightInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveRight);
	bool		moveUpInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveUp);
	bool		moveDownInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveDown);

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
		ForceXMVEC += XMVectorAdd(ForceXMVEC, XMLoadFloat3(&camera->GetUp()));
	}

	if (moveRightInput)
	{
		ForceXMVEC -= XMVectorAdd(ForceXMVEC, XMLoadFloat3(&camera->GetUp()));
	}

	if (moveUpInput)
	{
		ForceXMVEC += XMVectorAdd(ForceXMVEC,XMLoadFloat3(&camera->GetRight()));
	}

	if (moveDownInput)
	{
		ForceXMVEC -= XMVectorAdd(ForceXMVEC, XMLoadFloat3(&camera->GetRight()));
	}

	//normalize horizontal movement
	XMFLOAT3 force;
	XMStoreFloat3(&force, ForceXMVEC);

	PxVec3 forceVec{force.x,force.y,force.z};
	forceVec = forceVec.getNormalized();

	forceVec *= m_TorqueForce * timeStep;

	m_pCubeActor->addTorque(forceVec);
}

void TorqueScene::Draw() const
{

}

void TorqueScene::OnSceneActivated()
{
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"Scene Activated > \"%ls\"", GetName().c_str());
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"\t[INPUT > Reset='R' | Apply_Torque='Arrow Keys']");

	XMFLOAT3 cubeStartPos{ 0,m_CubeDimentions.y / 2.f + 1.f,0 }; // always start 1 unit in the air
	m_pCube->Translate(cubeStartPos.x, cubeStartPos.y, cubeStartPos.z);
	m_pCube->Rotate(0, 0, 0);
}

void TorqueScene::OnSceneDeactivated()
{
}
