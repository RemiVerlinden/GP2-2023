#include "stdafx.h"
#include "PortalTestScene.h"
#include "Base\Logger.h"

#include "Prefabs/Character.h"
#include "Materials/ColorMaterial.h"
#include "Materials/Portal/PortalMaterial.h"
#include "Components\PortalComponent.h"
#include "Prefabs\SpherePrefab.h"

void PortalTestScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	pBall1 = AddChild(new SpherePrefab(0.5f,10,XMFLOAT4(Colors::Orange)));
	pBall2 = AddChild(new SpherePrefab(0.5f, 10, XMFLOAT4(Colors::Blue)));

	//Create portals
	CreatePortals();

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
	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 0.f);

	//Simple Level
	const auto pLevelObject = AddChild(new GameObject());
	const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Labs/Week7/Meshes/SimpleLevel.ovm"));
	pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());

	const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Labs/Week7/Meshes/SimpleLevel.ovpt");
	pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ .5f,.5f,.5f })), *pDefaultMaterial);
	pLevelObject->GetTransform()->Scale(.5f, .5f, .5f);

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
//
//void PortalTestScene::InitializePortal(Portal portal)
//{
//	m_pPortals[portal] = AddChild(new GameObject());
//
//	const auto pLevelMesh = m_pPortals[portal]->AddComponent(new ModelComponent(L"Blender/Portal.ovm"));
//	pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());
//}

void PortalTestScene::CreatePortals()
{
	//create game objects where portals willl be stored
	m_pPortals[Portal::Orange] = AddChild(new GameObject());
	m_pPortals[Portal::Blue] = AddChild(new GameObject());

	// add the portal logic to the game objects
	for (GameObject* pPortal : m_pPortals)
	{
		pPortal->AddComponent(new PortalComponent());

		const auto plevelmesh = pPortal->AddComponent(new ModelComponent(L"blender/portal.ovm"));
		plevelmesh->SetMaterial(MaterialManager::Get()->CreateMaterial<PortalMaterial>());

		// add rigidbody
		RigidBodyComponent* pRigidBody = pPortal->AddComponent(new RigidBodyComponent(true));
		PxBoxGeometry colliderSize
		{
			2,
			1,
			0.1f
		};

		auto* pMaterial = PxGetPhysics().createMaterial(.0f, .0f, 0.0f);

		pRigidBody->AddCollider(PxBoxGeometry{ colliderSize }, *pMaterial, false, {0,1,0});

	}

	// link PortalComponent of each game object to each other
	PortalComponent* orangePortal = m_pPortals[Portal::Orange]->GetComponent<PortalComponent>();
	PortalComponent* bluePortal = m_pPortals[Portal::Blue]->GetComponent<PortalComponent>();

	orangePortal->SetLinkedPortal(bluePortal);
	bluePortal->SetLinkedPortal(orangePortal);
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
	XMVECTOR playerRotation = XMLoadFloat4(&m_pCharacter->GetTransform()->GetRotation());
	m_pPortals[portal]->GetTransform()->Rotate(playerRotation);
}


void PortalTestScene::OnGUI()
{
	m_pCharacter->DrawImGui();
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

	TransformComponent *pCamera1, *pCamera2;

	pCamera1 = m_pPortals[Portal::Orange]->GetComponent<PortalComponent>()->GetPortalCamera()->GetTransform();
	pCamera2 = m_pPortals[Portal::Blue]->GetComponent<PortalComponent>()->GetPortalCamera()->GetTransform();

	pBall1->GetTransform()->Translate(pCamera1->GetPosition());
	pBall2->GetTransform()->Translate(pCamera2->GetPosition());
}
