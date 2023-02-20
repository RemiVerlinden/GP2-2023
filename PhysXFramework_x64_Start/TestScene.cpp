#include "stdafx.h"
#include "TestScene.h"

#include "CubePosColorNorm.h"
#include "TorusPosColorNorm.h"
#include "CylinderPosColorNorm.h"

#include "Logger.h"

void TestScene::Initialize()
{
	m_pCube = new CubePosColorNorm(5.f, 2.f, 1.f, XMFLOAT4{Colors::LightGoldenrodYellow} );
	AddGameObject(m_pCube);

	m_pDefaultCube = new CubePosColorNorm(1.f, 1.f, 1.f );
	AddGameObject(m_pDefaultCube);

	const auto pTorus = new TorusPosColorNorm(9.f, 50, 1.f, 10, XMFLOAT4{Colors::ForestGreen});
	AddGameObject(pTorus);

	const auto pCylinder = new CylinderPosColorNorm(1.5f, 10, 4.f, XMFLOAT4{Colors::Blue});
	pTorus->AddChild(pCylinder);

	pCylinder->Translate(0.f, 8.f,0.f);

	// Camera
	m_SceneContext.GetCamera()->SetPosition(XMFLOAT3{ 0.f,10.f,-25.f });
	m_SceneContext.GetCamera()->SetForward(XMFLOAT3{0.f,-0.3f,1.f});

	// Input
	InputAction moveLeft	= InputAction{ InputIds::moveLeft, InputTriggerState::down, VK_LEFT,-1,XINPUT_GAMEPAD_DPAD_LEFT };
	InputAction moveRight	= InputAction{ InputIds::moveRight, InputTriggerState::down, VK_RIGHT,-1,XINPUT_GAMEPAD_DPAD_RIGHT };
	InputAction moveUp		= InputAction{ InputIds::moveUp, InputTriggerState::down, VK_UP,-1,XINPUT_GAMEPAD_DPAD_UP };
	InputAction moveDown	= InputAction{ InputIds::moveDown, InputTriggerState::down, VK_DOWN,-1,XINPUT_GAMEPAD_DPAD_DOWN };
	m_SceneContext.GetInput()->AddInputAction(moveLeft);
	m_SceneContext.GetInput()->AddInputAction(moveRight);
	m_SceneContext.GetInput()->AddInputAction(moveUp);
	 m_SceneContext.GetInput()->AddInputAction(moveDown);

}

void TestScene::Update()
{
	float gameTime = GetSceneContext().GetGameTime()->GetTotal();
	m_pCube->RotateDegrees(0.f,90.f * gameTime,0.f );

	// Input
	float		deltaT{ m_SceneContext.GetGameTime()->GetElapsed() };
	XMFLOAT3	translation{};
	const float moveSpeed{ 10.f * deltaT};
	bool		moveLeftInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveLeft);
	bool		moveRightInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveRight);
	bool		moveUpInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveUp);
	bool		moveDownInput = m_SceneContext.GetInput()->IsActionTriggered(InputIds::moveDown);

	if (moveLeftInput)
	{
		translation.x -= moveSpeed;
	}

	if (moveRightInput)
	{
		translation.x += moveSpeed;
	}

	if (moveUpInput)
	{
		translation.y += moveSpeed;
	}

	if (moveDownInput)
	{
		translation.y -= moveSpeed;
	}

	const XMVECTOR currPos = XMLoadFloat3( &m_pCube->GetPosition() );
	
	const XMVECTOR newPos = XMVectorAdd(currPos, XMLoadFloat3(&translation));

	XMFLOAT3 newTranslation{};
	XMStoreFloat3(&newTranslation, newPos);
	m_pCube->Translate(newTranslation.x, newTranslation.y, newTranslation.z);
}

void TestScene::Draw() const
{

}

void TestScene::OnSceneActivated()
{
	Logger::GetInstance()->LogFormat(LogLevel::Info, L"Scene Activated > \"%ls\"", GetName().c_str());
}

void TestScene::OnSceneDeactivated()
{
}
