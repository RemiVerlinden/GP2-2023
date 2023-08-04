#include "stdafx.h"
#include "PortalTestScene.h"
#include "Base\Logger.h"
#include "Prefabs/Character.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Portal/PortalMaterial.h"
#include "Components\PortalComponent.h"
#include "Prefabs\SpherePrefab.h"
#include "Prefabs\CubePrefab.h"
#include "Portal\MapLoader.h"
#include "SceneInputDefines\PortalInput.h"
#include "Prefabs\BoneObject.h"

void PortalTestScene::Initialize()
{
	m_SceneContext.settings.showInfoOverlay = true;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = true;


	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	//GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = Input::CharacterMoveForward;
	characterDesc.actionId_MoveBackward = Input::CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = Input::CharacterMoveLeft;
	characterDesc.actionId_MoveRight = Input::CharacterMoveRight;
	characterDesc.actionId_Jump = Input::CharacterJump;

	auto pBoneMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	auto pBone = new BoneObject(pBoneMaterial, 1);
	AddChild(pBone);

	m_pCharacter = AddChild(new Character(characterDesc));
	//m_pCharacter->GetTransform()->Translate(-35,10,7.5f);
	m_pCharacter->GetTransform()->Translate(0,1,0);

	//Create portals
	CreatePortals(m_pCharacter->GetCameraComponent());
	std::array<PortalComponent*, 2> portalComponents;
	portalComponents[0] = m_pPortals[0]->GetComponent<PortalComponent>();
	portalComponents[1] = m_pPortals[1]->GetComponent<PortalComponent>();

	PortalRenderer::Get()->InitializePortalComponents(portalComponents);


	//MapLoader maploader{ *this };
	//maploader.LoadMap(L"chamber02");

	//MapLoader::InteractiveElements& interactiveElements = maploader.GetInteractiveElements();
	//m_pCube = interactiveElements.cubes[0];
	//m_pDoor = interactiveElements.doors[0]->GetComponent<DoorComponent>();

	//interactiveElements.buttons[0]->GetComponent<ButtonAnimComponent>()->AddInteractionComponent(m_pDoor);


	m_pCharacter->InitializeCharacterMeshes();


	pBone->GetTransform()->Rotate(0, 0, 0);
	pBone->GetTransform()->Translate(2, 1, 1);

	//Input
	auto inputAction = InputAction(Input::CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Input::CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Input::CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Input::CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Input::CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Input::FirePrimary, InputState::pressed, -1, VK_XBUTTON1, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Input::FireSecondary, InputState::pressed, -1, VK_RBUTTON, XINPUT_GAMEPAD_LEFT_SHOULDER);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

void PortalTestScene::CreatePortals(CameraComponent* playerCamera)
{
	if (playerCamera == nullptr)
	{
		assert(false);
	}
	//create game objects where portals willl be stored
	m_pPortals[Portal::Orange] = AddChild(new GameObject());
	m_pPortals[Portal::Blue] = AddChild(new GameObject());

	// add the portal logic to the game objects
	for (UINT currentPortal = 0; currentPortal < m_pPortals.size(); ++currentPortal)
	{
		GameObject* pPortal = m_pPortals.at(currentPortal);
		PortalComponent* pPortalComponent = pPortal->AddComponent(new PortalComponent(m_pCharacter, (bool)currentPortal));

		const auto pPortalMesh = pPortal->AddComponent(new ModelComponent(L"blender/portal2.ovm"));

		PortalMaterial* pPortalMaterial;

		if ((PortalRenderer::Portal)currentPortal == PortalRenderer::Portal::blue)
			pPortalMaterial = MaterialManager::Get()->CreateMaterial<BluePortalMaterial>();
		else
			pPortalMaterial = MaterialManager::Get()->CreateMaterial<OrangePortalMaterial>();

		pPortalMaterial->SetPortalComponent(pPortalComponent);

		pPortalMesh->SetMaterial(pPortalMaterial);

		// add rigidbody
		RigidBodyComponent* pRigidBody = pPortal->AddComponent(new RigidBodyComponent(true));
		PxBoxGeometry colliderSize
		{
			1.28f,
			2.16f,
			0.1f
		};

		auto* pMaterial = PxGetPhysics().createMaterial(.0f, .0f, 0.0f);

		pRigidBody->AddCollider(PxBoxGeometry{ colliderSize }, *pMaterial, false, { 0,0,0 });

	}

	// link PortalComponent of each game object to each other
	PortalComponent* orangePortal = m_pPortals[Portal::Orange]->GetComponent<PortalComponent>();
	PortalComponent* bluePortal = m_pPortals[Portal::Blue]->GetComponent<PortalComponent>();

	orangePortal->SetLinkedPortal(bluePortal);
	bluePortal->SetLinkedPortal(orangePortal);

	orangePortal->GetTransform()->TranslateWorld({3, -10.f, -2});
	bluePortal->GetTransform()->TranslateWorld({ -3, -10, -2 });

}

void PortalTestScene::MovePortal(Portal portal)
{
	if (portal >= PortalsCount || portal < 0)
	{
		Logger::LogError(L"{} is not a valid Portal ID", (int)portal);
		assert(false);
	}

	XMFLOAT3 newPortalPos = m_pCharacter->GetCameraComponent()->GetTransform()->GetWorldPosition();
	XMFLOAT3 cameraForwardVec = m_pCharacter->GetCameraComponent()->GetTransform()->GetForward();

	const float distanceFromCamera = 5.f;

	XMVECTOR forwardVec = XMLoadFloat3(&cameraForwardVec) * distanceFromCamera;
	XMFLOAT3 forwardFloat3;
	XMStoreFloat3(&forwardFloat3, forwardVec);

	newPortalPos.x += forwardFloat3.x;
	newPortalPos.y += forwardFloat3.y;
	newPortalPos.z += forwardFloat3.z;
	//newPortalPos.y -= 1.5f;

	m_pPortals[portal]->GetTransform()->Translate(newPortalPos);
	
	XMFLOAT4 rotation = m_pCharacter->GetCameraComponent()->GetTransform()->GetWorldRotation();
	if (portal == Orange)
	{
		XMVECTOR axis = XMVectorSet(0, 1, 0, 0);
		float angle = XM_PI;
		XMVECTOR rotationVec = XMQuaternionRotationAxis(axis, angle);
		XMVECTOR newRotation = XMQuaternionMultiply(rotationVec, XMLoadFloat4(&rotation));
		XMFLOAT4 newRotationFloat4;
		XMStoreFloat4(&newRotationFloat4, newRotation);
		rotationVec = XMQuaternionNormalize(newRotation);
		XMStoreFloat4(&rotation, newRotation);
	}
	m_pPortals[portal]->GetTransform()->RotateWorld(XMLoadFloat4(&rotation));

	m_pPortals[portal]->GetComponent<PortalComponent>()->SetHasPortalMoved(true);
}

void PortalTestScene::OnGUI()
{
	m_pCharacter->DrawImGui();


	ImGui::Begin("PortalTestScene");


	ImGui::End();
}

void PortalTestScene::Update()
{
	if (m_SceneContext.pInput->IsMouseButton(InputState::pressed, VK_RBUTTON))
	{
		MovePortal(Orange);
	}
	if (m_SceneContext.pInput->IsMouseButton(InputState::pressed, VK_XBUTTON1) || m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'R'))
	{
		MovePortal(Blue);
	}
	//if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'K'))
	//{
	//	const auto pCameraTransform = m_SceneContext.pCamera->GetTransform();
	//	m_SceneContext.pLights->SetDirectionalLight(pCameraTransform->GetPosition(), pCameraTransform->GetForward());
	//}

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'F'))
	{
		XMFLOAT3 characterPos = m_pCharacter->GetTransform()->GetPosition();
		XMFLOAT3 newCubePos = characterPos;
		newCubePos.y += 3;
		m_pCube->GetTransform()->Translate(newCubePos);
	}

	// group0 is the player and environment, group1 is for all objects that ignore player, group2 is for interaction with the button and some trigger stuff
	// group3 will be for all objects that are portalable
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'G'))
	if (const auto pPickedObject = m_SceneContext.pCamera->Pick(CollisionGroup::Group0 | CollisionGroup::Group1 | CollisionGroup::Group2))
	{
		//delete hit object from scene
		std::wcout << L"hit object: " << pPickedObject->GetTag() << std::endl;
	}
}
