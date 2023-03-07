//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"


ComponentTestScene::ComponentTestScene() :
	GameScene(L"ComponentTestScene") {}

void ComponentTestScene::Initialize()
{
	auto& physx = PxGetPhysics();
	auto pBouncyMaterial = physx.createMaterial(.5f, .5f, 1.f);


	// Ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	// Red Sphere (GROUP 0) (Ignore group 1 & 2)
	{
		auto pSphereRed = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Red });
		AddChild(pSphereRed);
		pSphereRed->GetTransform()->Translate(0, 30, 0);

		auto pRigidBody = pSphereRed->AddComponent(new RigidBodyComponent(false));

		pRigidBody->AddCollider(PxSphereGeometry{ 1 }, *pBouncyMaterial);
		pRigidBody->SetCollisionGroup(CollisionGroup::Group0);
		pRigidBody->SetCollisionIgnoreGroups(CollisionGroup::Group1 | CollisionGroup::Group2);
	}

	// Green Sphere (Group 1)
	{
		auto pSphereGreen = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Green });
		AddChild(pSphereGreen);
		pSphereGreen->GetTransform()->Translate(0.5f, 20, 0);

		auto pRigidBody = pSphereGreen->AddComponent(new RigidBodyComponent(false));

		pRigidBody->AddCollider(PxSphereGeometry{ 1 }, *pBouncyMaterial);
		pRigidBody->SetCollisionGroup(CollisionGroup::Group1);
		
		pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);
	}

	// Blue Sphere (Group 2)
	{
		auto pSphereBlue = new SpherePrefab(1, 10, XMFLOAT4{ Colors::Blue });
		AddChild(pSphereBlue);
		pSphereBlue->GetTransform()->Translate(0, 10, 0);

		auto pRigidBody = pSphereBlue->AddComponent(new RigidBodyComponent(false));

		pRigidBody->AddCollider(PxSphereGeometry{ 1 }, *pBouncyMaterial);
		pRigidBody->SetCollisionGroup(CollisionGroup::Group2);
	}


	Logger::LogInfo(L"Welcome, you are now situated in the Component Test Scene!");
}

void ComponentTestScene::Update()
{
	//Optional
}

void ComponentTestScene::Draw()
{
	//Optional
}

void ComponentTestScene::OnGUI()
{
}
