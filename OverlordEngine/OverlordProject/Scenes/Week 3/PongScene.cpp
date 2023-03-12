//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PongScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"


PongScene::PongScene() :
	GameScene(L"PongScene") {}

void PongScene::Initialize()
{
	auto& physx = PxGetPhysics();
	auto* pMaterial = physx.createMaterial(.0f, .0f, 1.f);


	// CAMERA 
	{
		CameraComponent* pCameraComponent = GetActiveCamera();
		pCameraComponent->GetTransform()->Translate(0,30,0);

		FreeCamera* pActiveCamera = dynamic_cast<FreeCamera*>(pCameraComponent->GetGameObject());
		pActiveCamera->SetRotation(90,0);
	}
	// BALL
	{
		m_pBall = new SpherePrefab(BallProps.radius, 32, BallProps.color);
		AddChild(m_pBall);

		m_pBall->GetTransform()->Translate(BallProps.spawnPos);
	}
	// PADDLES
	{
		// not sure if this is very readable, I just wanna experiment with lambdas

		auto CreatePaddle = [this]( bool spawnPaddleLeftSide) -> GameObject*
		{
			GameObject* pPaddle = new CubePrefab(PaddleProps.size, PaddleProps.color);
			AddChild(pPaddle);

			XMFLOAT3 spawnPos = PaddleProps.centerSpawnPos; // start pos
			spawnPos.x += PaddleProps.horizontalSpawnOffset; // both paddles have an offset from the center
			spawnPos.x *= (spawnPaddleLeftSide) ? -1 : 1; // decide wether we spawn the paddle on the left, or right side

			pPaddle->GetTransform()->Translate(spawnPos);

			return pPaddle;
		};

		m_Paddles.first = CreatePaddle(true);
		m_Paddles.second = CreatePaddle(false);
	}

	// LEVEL BOUNDS (trying special indenting maybe this is more readable?)
	{
		LevelBoundsProperties& props	= LevelBoundsProps;

		// BOUND template
		auto CreateVerticalBound		= [this, pMaterial, props](float boundsVerticalOffset)
		{
			PxVec3 boundHalfExtents		= { props.levelBounds.x, props.boundsBoxHeight, props.boundsBoxWidth };
			{
				boundHalfExtents		/= 2;
			}
			PxVec3 boundsCenterPos		= { props.centerPos.x ,props.centerPos.y ,props.centerPos.z };
			{
				boundsCenterPos.z		+= boundsVerticalOffset;
			}


			const auto pActor			= PxGetPhysics().createRigidStatic(PxTransform{ boundsCenterPos });
			PxRigidActorExt				::createExclusiveShape(*pActor, PxBoxGeometry{ boundHalfExtents }, *pMaterial);

			GetPhysxProxy()->AddActor(*pActor);
		};

		float verticalOffset			= props.levelBounds.z / 2;

		CreateVerticalBound(-verticalOffset);
		CreateVerticalBound(verticalOffset);
	}

	// TRIGGERS 
	{
		LevelBoundsProperties& props = LevelBoundsProps;

		auto CreateHorizontalTrigger = [this, pMaterial, props](float boundsHorizontalOffset)
		{
			PxVec3 boundHalfExtents = { props.boundsBoxWidth, props.boundsBoxHeight, props.levelBounds.z };
			{
				boundHalfExtents.z += props.boundsBoxWidth;
				boundHalfExtents /= 2;
			}
			PxVec3 boundsCenterPos = { props.centerPos.x ,props.centerPos.y ,props.centerPos.z };
			{
				boundsCenterPos.x += boundsHorizontalOffset;
				boundsCenterPos.x += props.boundsBoxWidth / 2.f * ((boundsHorizontalOffset > 0) ? 1 : -1);
			}


			const auto pActor = PxGetPhysics().createRigidStatic(PxTransform{ boundsCenterPos });
			PxRigidActorExt::createExclusiveShape(*pActor, PxBoxGeometry{ boundHalfExtents }, *pMaterial);
			
			GetPhysxProxy()->AddActor(*pActor);
		};

		float horizontalOffset = props.levelBounds.x / 2;

		CreateHorizontalTrigger(-horizontalOffset);
		CreateHorizontalTrigger(horizontalOffset);
	}
	
}

void PongScene::Update()
{
	//Optional
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
