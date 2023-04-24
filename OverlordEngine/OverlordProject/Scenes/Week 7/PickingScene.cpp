#include "stdafx.h"
#include "PickingScene.h"
#include "Prefabs\FreeCamera.h"
#include "Prefabs/SpherePrefab.h"

void PickingScene::Initialize()
{
	//scene settings
	m_SceneContext.settings.drawGrid = false;

	//basic physics material
	const auto pMaterial = PxGetPhysics().createMaterial(.5f, .5f, .5f);

	//create sphere grid
	constexpr int amount{ 5 };
	//for each column
	for (auto col{ -amount }; col <= amount; ++col)
	{
		//for each row
		for (auto row{ -amount }; row <= amount; ++row)
		{
			//create sphere
			const auto pSphere = AddChild(new SpherePrefab(4.f));

			//attach rigid body
			const auto pRigidBody = pSphere->AddComponent(new RigidBodyComponent(true));
			//add collider
			pRigidBody->AddCollider(PxSphereGeometry{ 4.f }, *pMaterial);

			//position in scene
			pSphere->GetTransform()->Translate(20.f * col, 0.f, 20.f * row);
		}
	}
}

void PickingScene::Update()
{

	//if right click is pressed
	if (InputManager::IsMouseButton(InputState::pressed, VK_RBUTTON))
	{
		//if raycast from mouse position returns valid hit result
		if (const auto pPickedObject = m_SceneContext.pCamera->Pick())
		{
			//delete hit object from scene
			RemoveChild(pPickedObject, true);
		}
	}
}
