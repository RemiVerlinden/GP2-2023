//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include <random>

PongScene::PongScene() :
	GameScene(L"PongScene") {}

void PongScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;

	auto& physx = PxGetPhysics();
	auto* pMaterial = physx.createMaterial(.0f, .0f, 1.02f);


	// CAMERA 
	{
		const auto pFixedCamera = new FixedCamera();
		pFixedCamera->GetTransform()->Rotate(90,0,0,true);
		pFixedCamera->GetTransform()->Translate(0, 30, 0);
		AddChild(pFixedCamera);
		SetActiveCamera(pFixedCamera->GetComponent<CameraComponent>());
	}

	// BACKGROUND COLOR
	{
		const XMFLOAT4 blackColor{ 0.f,0.f,0.f,1.f };
		m_SceneContext.settings.clearColor = blackColor;
	}
	// BALL
	{
		m_pBall = new SpherePrefab(BallProps.radius, 32, BallProps.color);
		AddChild(m_pBall);
		m_pBall->GetTransform()->Translate(BallProps.spawnPos);

		RigidBodyComponent* pRigidBody = m_pBall->AddComponent(new RigidBodyComponent());
		pRigidBody->AddCollider(PxSphereGeometry{ BallProps.radius }, *pMaterial);


		// set constraints
		pRigidBody->SetConstraint(RigidBodyConstraint::AllRot, false);
		pRigidBody->SetConstraint(RigidBodyConstraint::TransY, false);

		m_pBallRigidBody = pRigidBody;
	}
	// PADDLES
	{
		// not sure if this is very readable, I just wanna experiment with lambdas

		auto CreatePaddle = [this, pMaterial]( bool spawnPaddleLeftSide) -> GameObject*
		{
			GameObject* pPaddle = new CubePrefab(PaddleProps.size, PaddleProps.color);
			AddChild(pPaddle);

			XMFLOAT3 spawnPos = PaddleProps.centerSpawnPos; // start pos
			spawnPos.x += PaddleProps.horizontalSpawnOffset; // both paddles have an offset from the center
			spawnPos.x *= (spawnPaddleLeftSide) ? -1 : 1; // decide wether we spawn the paddle on the left, or right side

			pPaddle->GetTransform()->Translate(spawnPos);

			// add rigidbody
			RigidBodyComponent* pRigidBody = pPaddle->AddComponent(new RigidBodyComponent( true));
			PxBoxGeometry paddleColliderSize
			{
				PaddleProps.size.x / 2,
				PaddleProps.size.y / 2,
				PaddleProps.size.z / 2
			};
			pRigidBody->AddCollider(PxBoxGeometry{ paddleColliderSize }, *pMaterial);



			return pPaddle;
		};

		m_Paddles.first = CreatePaddle(true);
		m_Paddles.second = CreatePaddle(false);
	}

	// LEVEL BOUNDS (trying special indenting maybe this is more readable?)
	{
		LevelBoundsProperties& props	= LevelBoundsProps;

		// BOUND template
		auto CreateHorizontalBounds		= [this, pMaterial, props](float boundsVerticalOffset)
		{
			PxVec3 boundHalfExtents		= { props.levelBounds.x, props.boundsBoxHeight, props.boundsBoxWidth };
				boundHalfExtents		/= 2;

			XMFLOAT3 boundsCenterPos	= props.centerPos;
				boundsCenterPos.z		+= boundsVerticalOffset;

			GameObject* pVertialBound = new GameObject();
			AddChild(pVertialBound);
			pVertialBound->GetTransform()->Translate(boundsCenterPos);

			RigidBodyComponent* pRigidBody = pVertialBound->AddComponent(new RigidBodyComponent(true));
			pRigidBody->AddCollider(PxBoxGeometry{ boundHalfExtents }, *pMaterial);

			//const auto pActor			= PxGetPhysics().createRigidStatic(PxTransform{ boundsCenterPos });
			//PxRigidActorExt				::createExclusiveShape(*pActor, PxBoxGeometry{ boundHalfExtents }, *pMaterial);

			//GetPhysxProxy()->AddActor(*pActor);
		};

		float verticalOffset			= props.levelBounds.z / 2;

		CreateHorizontalBounds(-verticalOffset);
		CreateHorizontalBounds(verticalOffset);
	}

	// TRIGGERS 
	{
		LevelBoundsProperties& props = LevelBoundsProps;

		auto CreateHorizontalTrigger = [this, pMaterial, props](float boundsHorizontalOffset)
		{
			PxVec3 triggerHalfExtents = { props.boundsBoxWidth, props.boundsBoxHeight, props.levelBounds.z };
			{
				triggerHalfExtents.z += props.boundsBoxWidth;
				triggerHalfExtents /= 2;
			}
			XMFLOAT3 triggerCenterPos = props.centerPos;
			{
				triggerCenterPos.x += boundsHorizontalOffset;
				triggerCenterPos.x += props.boundsBoxWidth / 2.f * ((boundsHorizontalOffset > 0) ? 1 : -1);
			}
			GameObject* pHozizontalTrigger = new GameObject();
			AddChild(pHozizontalTrigger);
			pHozizontalTrigger->GetTransform()->Translate(triggerCenterPos);

			RigidBodyComponent* pRigidBody = pHozizontalTrigger->AddComponent(new RigidBodyComponent(true));
			pRigidBody->AddCollider(PxBoxGeometry{ triggerHalfExtents }, * pMaterial, true);

			pHozizontalTrigger->SetOnTriggerCallBack([this](GameObject*, GameObject*, PxTriggerAction action)
			{
				if (action == PxTriggerAction::ENTER)
				{
					ResetBall();
				}
			});

			//const auto pActor = PxGetPhysics().createRigidStatic(PxTransform{ boundsCenterPos });
			//PxRigidActorExt::createExclusiveShape(*pActor, PxBoxGeometry{ boundHalfExtents }, *pMaterial);
			//
			//GetPhysxProxy()->AddActor(*pActor);
		};

		float horizontalOffset = props.levelBounds.x / 2;

		CreateHorizontalTrigger(-horizontalOffset);
		CreateHorizontalTrigger(horizontalOffset);
	}

	// Input
	{
		InitializePaddleInput();
	}
	
}

float RandomFloat(float min, float max)
{
	static std::mt19937 generator(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(generator);
}

void PongScene::Update()
{
	const float deltaTime{ m_SceneContext.pGameTime->GetElapsed() };

	auto leftPaddle = m_Paddles.first;
	auto rightPaddle = m_Paddles.second;


	if (m_SceneContext.pInput->IsActionTriggered(PongInput::PLAYER1_UP))
	{
		MoveCube(leftPaddle, PaddleProps.speed * deltaTime);
	}

	if (m_SceneContext.pInput->IsActionTriggered(PongInput::PLAYER1_DOWN))
	{
		MoveCube(leftPaddle, -PaddleProps.speed * deltaTime);;
	}

	if (m_SceneContext.pInput->IsActionTriggered(PongInput::PLAYER2_UP))
	{
		MoveCube(rightPaddle, PaddleProps.speed* deltaTime);
	}

	if (m_SceneContext.pInput->IsActionTriggered(PongInput::PLAYER2_DOWN))
	{
		MoveCube(rightPaddle, -PaddleProps.speed * deltaTime);
	}

	//if (!m_GameStarted && m_SceneContext.pInput->IsActionTriggered(PongInput::STARTGAME))
	//{
	//	m_GameStarted = true;

	//	// Set the maximum angle in degrees
	//	float maxAngleDegrees = 45.0f;
	//	float maxAngleRadians = maxAngleDegrees * DirectX::XM_PI / 180.0f;

	//	// Generate random angles within the allowed range
	//	float randomAngleZ = RandomFloat(-maxAngleRadians, maxAngleRadians);

	//	// Calculate the random force direction
	//	XMFLOAT3 randomForceDir{ 10.0f, 0.0f, 10.0f * std::tan(randomAngleZ) };
	//	XMVECTOR forceDirection = XMLoadFloat3(&randomForceDir);
	//	forceDirection = XMVector3Normalize(forceDirection) * BallProps.speed;
	//	if(rand() % 2) forceDirection = -forceDirection;
	//	XMStoreFloat3(&randomForceDir, forceDirection);

	//	// Apply the force
	//	m_pBallRigidBody->AddForce(randomForceDir, PxForceMode::Enum::eIMPULSE);
	//}



	if (!m_GameStarted && m_SceneContext.pInput->IsActionTriggered(PongInput::STARTGAME))
	{
		m_GameStarted = true;

		// Set the minimum and maximum angle in degrees
		float minAngleDegrees = 15.0f;
		float maxAngleDegrees = 45.0f;
		float minAngleRadians = minAngleDegrees * DirectX::XM_PI / 180.0f;
		float maxAngleRadians = maxAngleDegrees * DirectX::XM_PI / 180.0f;

		// Generate random angles within the allowed range
		float randomAngleZ = RandomFloat(minAngleRadians, maxAngleRadians);

		// Randomly choose the vertical direction (positive or negative)
		randomAngleZ *= (RandomFloat(0.0f, 1.0f) > 0.5f) ? 1 : -1;

		// Calculate the random force direction
		XMFLOAT3 randomForceDir{ 1.0f, 0.0f, std::tan(randomAngleZ) };
		XMVECTOR forceDirection = XMLoadFloat3(&randomForceDir);
		forceDirection = XMVector3Normalize(forceDirection) * BallProps.speed;

		// Randomly choose the horizontal direction (positive or negative)
		if (rand() % 2) forceDirection = -forceDirection;
		XMStoreFloat3(&randomForceDir, forceDirection);

		// Apply the force
		m_pBallRigidBody->AddForce(randomForceDir, PxForceMode::Enum::eIMPULSE);
	}
}

void PongScene::Draw()
{
	//Optional
}

void PongScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}

void PongScene::OnSceneActivated()
{
	ResetBall();
}


void PongScene::InitializePaddleInput()
{
	m_SceneContext.pInput->AddInputAction(InputAction{ PongInput::PLAYER1_UP,	InputState::down,	'W',	-1,		XINPUT_GAMEPAD_DPAD_UP,GamepadIndex::playerTwo });
	m_SceneContext.pInput->AddInputAction(InputAction{ PongInput::PLAYER1_DOWN,	InputState::down,	'S',	-1,		XINPUT_GAMEPAD_DPAD_DOWN,GamepadIndex::playerTwo });
	m_SceneContext.pInput->AddInputAction(InputAction{ PongInput::PLAYER2_UP,	InputState::down,	VK_UP,	-1,		XINPUT_GAMEPAD_DPAD_UP,GamepadIndex::playerOne });
	m_SceneContext.pInput->AddInputAction(InputAction{ PongInput::PLAYER2_DOWN,	InputState::down,	VK_DOWN,-1,		XINPUT_GAMEPAD_DPAD_DOWN,GamepadIndex::playerOne });
	m_SceneContext.pInput->AddInputAction(InputAction{ PongInput::STARTGAME,	InputState::pressed,VK_SPACE,-1,	XINPUT_GAMEPAD_BACK,GamepadIndex::playerOne });
	m_SceneContext.pInput->AddInputAction(InputAction{ PongInput::STARTGAME,	InputState::pressed,VK_SPACE,-1,	XINPUT_GAMEPAD_BACK,GamepadIndex::playerTwo });
}

void PongScene::MoveCube(GameObject* prefab, float distance)
{
	auto cubePos = prefab->GetTransform()->GetPosition();
	cubePos.z += distance;

	auto paddleHeight = PaddleProps.size.z;
	float maxOffset = (LevelBoundsProps.levelBounds.z - paddleHeight - LevelBoundsProps.boundsBoxWidth) / 2;
	float minHeight = LevelBoundsProps.centerPos.z - maxOffset;

	if (cubePos.z <= minHeight)
	{
		cubePos.z = minHeight;
	}

	auto maxHeight = LevelBoundsProps.centerPos.z + maxOffset;
	if (cubePos.z >= maxHeight)
	{
		cubePos.z = maxHeight;
	}
	prefab->GetTransform()->Translate(cubePos);
}

void PongScene::ResetBall()
{
	m_GameStarted = false;
	m_pBall->GetTransform()->Translate(0, 0, 0);
}
