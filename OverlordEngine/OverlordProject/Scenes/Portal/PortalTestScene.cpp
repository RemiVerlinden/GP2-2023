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

void PortalTestScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;


	/*	pBall1 = AddChild(new SpherePrefab(0.5f, 10, XMFLOAT4(Colors::Orange)));
	pBall2 = AddChild(new SpherePrefab(0.5f, 10, XMFLOAT4(Colors::Blue)));*/


	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(-35,1,7.5f);

	//Create portals
	CreatePortals(m_pCharacter->GetCameraComponent());
	std::array<PortalComponent*, 2> portalComponents;
	portalComponents[0] = m_pPortals[0]->GetComponent<PortalComponent>();
	portalComponents[1] = m_pPortals[1]->GetComponent<PortalComponent>();

	PortalRenderer::Get()->InitializePortalComponents(portalComponents);

	MapLoader maploader{ *this };
	maploader.LoadMap(L"chamber02");

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
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
		pPortal->AddComponent(new PortalComponent(m_pCharacter, (bool)currentPortal));

		const auto pPortalMesh = pPortal->AddComponent(new ModelComponent(L"blender/portal.ovm"));

		PortalMaterial* pPortalMaterial;

		if ((PortalRenderer::Portal)currentPortal == PortalRenderer::Portal::blue)
			pPortalMaterial = MaterialManager::Get()->CreateMaterial<BluePortalMaterial>();
		else
			pPortalMaterial = MaterialManager::Get()->CreateMaterial<OrangePortalMaterial>();

		pPortalMesh->SetMaterial(pPortalMaterial);

		// add rigidbody
		RigidBodyComponent* pRigidBody = pPortal->AddComponent(new RigidBodyComponent(true));
		PxBoxGeometry colliderSize
		{
			2,
			1,
			0.1f
		};

		auto* pMaterial = PxGetPhysics().createMaterial(.0f, .0f, 0.0f);

		pRigidBody->AddCollider(PxBoxGeometry{ colliderSize }, *pMaterial, false, { 0,1,0 });

	}

	// link PortalComponent of each game object to each other
	PortalComponent* orangePortal = m_pPortals[Portal::Orange]->GetComponent<PortalComponent>();
	PortalComponent* bluePortal = m_pPortals[Portal::Blue]->GetComponent<PortalComponent>();

	orangePortal->SetLinkedPortal(bluePortal);
	bluePortal->SetLinkedPortal(orangePortal);

	orangePortal->GetTransform()->TranslateWorld({-5, 0.f, 5});
	bluePortal->GetTransform()->TranslateWorld({ 5, 0, 5 });

	//orangePortal->GetTransform()->Rotate( 0,20,0 );
	//bluePortal->GetTransform()->Rotate(0,90,0 );
}

void PortalTestScene::MovePortal(Portal portal)
{
	if (portal >= PortalsCount || portal < 0)
	{
		Logger::LogError(L"{} is not a valid Portal ID", (int)portal);
		assert(false);
	}

	//if (!m_pPortals[portal])
	//	InitializePortal(portal);

	m_pPortals[portal]->GetTransform()->Translate(m_pCharacter->GetTransform()->GetPosition());
	//m_pPortals[portal]->GetTransform()->Translate(10,10,10);
	
	auto rotation = m_pCharacter->GetCameraComponent()->GetTransform()->GetWorldRotation();
	m_pPortals[portal]->GetTransform()->RotateWorld(XMLoadFloat4(&rotation));
	//m_pPortals[portal]->GetTransform()->Rotate(0, m_pCharacter->GetYaw(), 0);
}


void PortalTestScene::OnGUI()
{
	m_pCharacter->DrawImGui();

	static float scale = 1.f;

	ImGui::Begin("PortalTestScene");

	ImGui::DragFloat("Scale", &scale, 0.1f, 0.1f, 2.f);

	static XMFLOAT3 testObjectPos{};

	float pos[] = { testObjectPos.x, testObjectPos.y, testObjectPos.z };

	if (ImGui::SliderFloat3("blue portal camera rotation", pos, -XM_PIDIV2, XM_PIDIV2))
		testObjectPos = { pos[0], pos[1], pos[2] };

	XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYaw(pos[0], pos[1], pos[2]);

	m_pPortals[Portal::Blue]->GetComponent<PortalComponent>()->GetCameraRotator()->GetTransform()->Rotate(rotationQuat);



	float vectorArray[4] = {};
	XMStoreFloat4((XMFLOAT4*)vectorArray, rotationQuat);
	if (ImGui::InputFloat4("My Vector", vectorArray))
	{
	}

	ImGui::End();
}

void PortalTestScene::Update()
{
	if (m_SceneContext.pInput->IsMouseButton(InputState::pressed, VK_RBUTTON))
	{
		MovePortal(Blue);
	}
	if (m_SceneContext.pInput->IsMouseButton(InputState::pressed, VK_XBUTTON1))
	{
		MovePortal(Orange);
	}
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'K'))
	{
		const auto pCameraTransform = m_SceneContext.pCamera->GetTransform();
		m_SceneContext.pLights->SetDirectionalLight(pCameraTransform->GetPosition(), pCameraTransform->GetForward());
	}
}
