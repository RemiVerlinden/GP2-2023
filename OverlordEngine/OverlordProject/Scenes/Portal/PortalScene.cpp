#include "stdafx.h"
#include "PortalScene.h"
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
#include "Scenes\Portal\PortalMainMenu.h"
#include "Materials/Post/PostChromatic.h"
#include "Components\CameraComponent.h"
#include <random>

void PortalScene::Initialize()
{
	m_SceneContext.settings.showInfoOverlay = true;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	// SOUNDS
	{
		auto soundManager = SoundManager::Get();

		std::string shootOrangeSound = "Resources/Sounds/Weapon/portalgun_shoot_orange.wav";
		std::string shootBlueSound = "Resources/Sounds/Weapon/portalgun_shoot_blue.wav";

		std::string portalClose1Sound = "Resources/Sounds/Portals/portal_close1.wav";
		std::string portalClose2Sound = "Resources/Sounds/Portals/portal_close2.wav";
		std::string portalOpen1Sound = "Resources/Sounds/Portals/portal_open1.wav";
		std::string portalOpen2Sound = "Resources/Sounds/Portals/portal_open2.wav";
		std::string portalOpen3Sound = "Resources/Sounds/Portals/portal_open3.wav";

		std::string ambientMainSound = "Resources/Sounds/Ambient/ambience_base.wav";

		std::string ambientRandom1Sound = "Resources/Sounds/Ambient/cave_hit1.wav";
		std::string ambientRandom2Sound = "Resources/Sounds/Ambient/cave_hit2.wav";
		std::string ambientRandom3Sound = "Resources/Sounds/Ambient/cave_hit3.wav";
		std::string ambientRandom4Sound = "Resources/Sounds/Ambient/cave_hit4.wav";

		std::string ElevatorChimeSound = "Resources/Sounds/Ambient/portal_elevator_chime.wav";

		m_pSounds[SoundLibrary::FireOrangePortal] = soundManager->LoadSound(shootOrangeSound, false);
		m_pSounds[SoundLibrary::FireBluePortal] = soundManager->LoadSound(shootBlueSound, false);
		
		m_pSounds[SoundLibrary::PortalClose1] = soundManager->LoadSound(portalClose1Sound, false, true);
		m_pSounds[SoundLibrary::PortalClose2] = soundManager->LoadSound(portalClose2Sound, false, true);
		m_pSounds[SoundLibrary::PortalOpen1] = soundManager->LoadSound(portalOpen1Sound, false, true);
		m_pSounds[SoundLibrary::PortalOpen2] = soundManager->LoadSound(portalOpen2Sound, false, true);
		m_pSounds[SoundLibrary::PortalOpen3] = soundManager->LoadSound(portalOpen3Sound, false, true);

		m_pSounds[SoundLibrary::AmbientMain] = soundManager->LoadSound(ambientMainSound, true, false);
		SoundManager::Get()->Play2DSound(m_pSounds[SoundLibrary::AmbientMain], 0.1f, SoundManager::SoundChannel::Background, false);
		SoundManager::Get()->PauseChannelGroup(SoundManager::SoundChannel::Background);

		m_pSounds[SoundLibrary::AmbientRandom1] = soundManager->LoadSound(ambientRandom1Sound, false, false);
		m_pSounds[SoundLibrary::AmbientRandom2] = soundManager->LoadSound(ambientRandom2Sound, false, false);
		m_pSounds[SoundLibrary::AmbientRandom3] = soundManager->LoadSound(ambientRandom3Sound, false, false);
		m_pSounds[SoundLibrary::AmbientRandom4] = soundManager->LoadSound(ambientRandom4Sound, false, false);
		
		m_pSounds[SoundLibrary::ElevatorChime] = soundManager->LoadSound(ElevatorChimeSound, false, true);
		SoundManager::Get()->Play3DSound(m_pSounds[SoundLibrary::ElevatorChime], 1.f, { -48.4f, 8.1f, 7.4f }, SoundManager::SoundChannel::Background, false); // location of elevator start

	}
	// LIGHTS
	{
		std::vector<Light> lightsVec;
		Light light;
		light.PCFLevel = 8;
		//-0.1, 8.6, 8.6
		light.position = { -31.6f, 5.6f, 8.6f,0.f }; // center room small
		lightsVec.push_back(light);
		light.position = { 0.3f, 8.3f, 8.0f,0.f }; // center room big 
		light.farPlane = 30.f;
		lightsVec.push_back(light);
		light.position = { 0.1f, 9.1f, -2.f,0.f }; // center room big above pit
		light.farPlane = 20.f;
		lightsVec.push_back(light);
		light.position = { -48.3f, 4.3f, 7.7f,0.f }; // elevator beginning
		lightsVec.push_back(light);
		light.position = { 35.3f, 3.1f, 7.6f,0.f }; // elevator ending
		lightsVec.push_back(light);
		light.position = { 15.4f, 3.5f, 7.8f,0.f }; // behind door
		light.farPlane = 25.f;
		lightsVec.push_back(light);
		light.position = { 4.7f, 8.5f, 19.7f,0.f }; // big observation room bottom light
		light.farPlane = 15.f;
		light.PCFLevel = 1;
		lightsVec.push_back(light);
		//light.position = { 4.7f, 11.4f, 19.6f,0.f }; // big observation room top light
		//lightsVec.push_back(light);
		light.position = { -17.74f, 8.31f, -7.47f,0.f }; // small observation room bottom light
		light.farPlane = 7.f;
		lightsVec.push_back(light);
		//light.position = { -17.74f, 8.9f, -7.47f,0.f }; // small observation room top light
		//lightsVec.push_back(light);


		for (const Light& currentLight : lightsVec)
			m_SceneContext.pLights->AddLight(this, currentLight);
	}

	// CROSSHAIR
	{
		GameObject* pGo = new GameObject();
		m_pCrosshairComponent = pGo->AddComponent(new CrosshairComponent());
		pGo->GetTransform()->Translate(m_SceneContext.windowWidth / 2, m_SceneContext.windowHeight / 2, 0.5f);

		//AddChild(pGo);
	}

	// Get the menu scene
	m_pMenuScene = dynamic_cast<PortalMainMenu*>(SceneManager::Get()->GetScene(L"PortalMainMenu"));
	m_pMenuTexture = ContentManager::Load<TextureData>(L"Textures/UI/background.dds");
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


	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(-35, 1, 7.5f);
	//m_pCharacter->GetTransform()->Translate(0,1,0);

	//Create portals
	CreatePortals(m_pCharacter->GetCameraComponent());
	std::array<PortalComponent*, 2> portalComponents;
	portalComponents[0] = m_pPortals[0]->GetComponent<PortalComponent>();
	portalComponents[1] = m_pPortals[1]->GetComponent<PortalComponent>();

	PortalRenderer::Get()->InitializePortalComponents(portalComponents);


	MapLoader maploader{ *this };
	maploader.LoadMap(L"chamber02");

	MapLoader::InteractiveElements& interactiveElements = maploader.GetInteractiveElements();
	m_pCube = interactiveElements.cubes[0];
	m_pDoor = interactiveElements.doors[0]->GetComponent<DoorComponent>();

	interactiveElements.buttons[0]->GetComponent<ButtonAnimComponent>()->AddInteractionComponent(m_pDoor);


	m_pCharacter->InitializeCharacterMeshes();



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

	inputAction = InputAction(Input::FirePrimary, InputState::pressed, -1, VK_LBUTTON, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Input::FireSecondary, InputState::pressed, -1, VK_RBUTTON, XINPUT_GAMEPAD_LEFT_SHOULDER);
	m_SceneContext.pInput->AddInputAction(inputAction);


	// POST PROCESSING
	m_pPostChromatic = MaterialManager::Get()->CreateMaterial<PostChromatic>();
	AddPostProcessingEffect(m_pPostChromatic);
	m_pPostChromatic->SetIsEnabled(false);



}

PortalScene::~PortalScene()
{
	if (m_ScenePaused)
		RemoveChild(m_pMenuObject);
}

void PortalScene::CreatePortals(CameraComponent* playerCamera)
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
		pPortalMesh->SetCastShadows(false);
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

	orangePortal->GetTransform()->TranslateWorld({ -12.7614f, 2.22742f, 12.8245f });
	orangePortal->GetTransform()->Rotate(XMFLOAT4{ 0.f, -0.382683f, 0.f, 0.92388f });
	bluePortal->GetTransform()->TranslateWorld({ -16.6547f, 2.24675f, 0.01f });
}

void PortalScene::MovePortal(Portal portal)
{
	if (portal >= PortalsCount || portal < 0)
	{
		Logger::LogError(L"{} is not a valid Portal ID", (int)portal);
		assert(false);
	}

	if (portal == Portal::Blue) 
	{
		SoundManager::Get()->Play2DSound(m_pSounds[SoundLibrary::FireBluePortal],0.3f, SoundManager::SoundChannel::Weapon, true);
	}
	else
	{
		SoundManager::Get()->Play2DSound(m_pSounds[SoundLibrary::FireOrangePortal], 0.3f, SoundManager::SoundChannel::Weapon, true);
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

	const float offsetDistance = .01f; // Change this to the desired offset distance

	// just do raycast and place portal at hit point
	if (auto [pHitObject, hit] = m_SceneContext.pCamera->CrosshairRaycast(CollisionGroup::Group0 | CollisionGroup::Group1 | CollisionGroup::Group2); pHitObject != nullptr)
	{
		// Default portal facing direction
		XMVECTOR defaultDir = XMVectorSet(0, 0, 1, 0); // Assuming +Z as default

		// Compute rotation axis and angle
		XMVECTOR hitNormal = XMLoadFloat3(&hit.normal);

		// If the portal is orange, invert the normal vector.
		if (portal == Orange)
		{
			hitNormal = XMVectorNegate(hitNormal);
		}

		XMVECTOR rotationAxis = XMVector3Cross(defaultDir, hitNormal);
		float rotationAngle = acosf(XMVectorGetX(XMVector3Dot(defaultDir, hitNormal)));

		// Check for a nearly zero-length rotation axis, which indicates parallel or antiparallel vectors
		XMVECTOR axisLength = XMVector3Length(rotationAxis);
		if (XMVectorGetX(axisLength) < 1e-6f)
		{ // 1e-6f is a small threshold; adjust as needed
			if (XMVectorGetX(XMVector3Dot(defaultDir, hitNormal)) < 0)
			{
				// Vectors are antiparallel
				rotationAxis = XMVectorSet(0, 1, 0, 0); // You can choose any perpendicular axis here
				rotationAngle = XM_PI; // 180 degrees
			}
			else
			{
				// Vectors are parallel
				// No need to rotate in this case, but for safety, you can set to identity
				rotationAngle = 0.0f;
				rotationAxis = XMVectorSet(1, 0, 0, 0); // Arbitrary since angle is zero
			}
		}

		// Create rotation quaternion
		XMVECTOR rotationQuat = XMQuaternionRotationAxis(rotationAxis, rotationAngle);

		// Offset the portal position slightly away from the wall
		XMVECTOR offset = XMVectorScale(XMLoadFloat3(&hit.normal), offsetDistance); // Notice we use the original normal for the offset, not the possibly negated one
		XMVECTOR finalPosition = XMVectorAdd(XMLoadFloat3(&hit.position), offset);


		XMFLOAT3 finalPositionFloat3;
		XMStoreFloat3(&finalPositionFloat3, finalPosition);

		// PLAY CLOSING PORTAL SOUND AT OLD LOCATION AND PLAY OPEN PORTAL SOUND AT NEW LOCATION
		{
			FMOD::Sound* pSound = nullptr;

			pSound = (m_SceneContext.frameNumber % 2) ? m_pSounds[SoundLibrary::PortalClose1] : m_pSounds[SoundLibrary::PortalClose2];
			SoundManager::Get()->Play3DSound(pSound,1.f, m_pPortals[portal]->GetTransform()->GetWorldPosition(), SoundManager::SoundChannel::Portals, true);
			pSound = m_pSounds[static_cast<SoundLibrary>((int)SoundLibrary::PortalOpen1 + (m_SceneContext.frameNumber % 3))];
			SoundManager::Get()->Play3DSound(pSound,1.f, finalPositionFloat3, SoundManager::SoundChannel::Portals, true);
		}

		m_pPortals[portal]->GetTransform()->Rotate(rotationQuat);
		m_pPortals[portal]->GetTransform()->TranslateWorld(finalPositionFloat3);


	}

	m_pPortals[portal]->GetComponent<PortalComponent>()->SetHasPortalMoved(true); // this will make the portal opening animation play
}

void PortalScene::PlayRandomBackgroundNoise()
{
	float deltaTime = m_SceneContext.pGameTime->GetElapsed();

	// Static variables will maintain their values across multiple function calls
	static float m_TimeSinceLastSound = 0.0f;
	static float m_NextSoundInterval = Random::GenerateRandomInterval(7,15);

	m_TimeSinceLastSound += deltaTime;

	if (m_TimeSinceLastSound >= m_NextSoundInterval)
	{
		// Reset the counters
		m_TimeSinceLastSound = 0.0f;
		m_NextSoundInterval = Random::GenerateRandomInterval(7, 15);

		// Play a random sound
		int soundIndex = static_cast<int>(SoundLibrary::AmbientRandom1);
		soundIndex += m_SceneContext.frameNumber % 4;

		SoundLibrary currentChannel = static_cast<SoundLibrary>(soundIndex);
		SoundManager::Get()->Play2DSound(m_pSounds[currentChannel], 0.07f, SoundManager::SoundChannel::Background, true);
	}
}



void PortalScene::OnGUI()
{
	m_pCharacter->DrawImGui();


	ImGui::Begin("PortalTestScene");


	ImGui::End();
}

void PortalScene::Update()
{

	if (m_FirePortalInUpdate != Portal::None)
	{
		MovePortal(m_FirePortalInUpdate);
		m_pCrosshairComponent->SetLastPortalFired(m_FirePortalInUpdate);
		m_FirePortalInUpdate = Portal::None;
	}
	//if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'K'))
	//{
	//	const auto pCameraTransform = m_SceneContext.pCamera->GetTransform();
	//	m_SceneContext.pLights->SetDirectionalLight(pCameraTransform->GetPosition(), pCameraTransform->GetForward());
	//}

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'E'))
	{
		XMFLOAT3 characterPos = m_pCharacter->GetTransform()->GetPosition();
		XMFLOAT3 newCubePos = characterPos;
		newCubePos.y += 3;
		m_pCube->GetTransform()->Translate(newCubePos);
	}
	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'P'))
		m_pPostChromatic->SetIsEnabled(!m_pPostChromatic->IsEnabled());


	PlayRandomBackgroundNoise();
	UpdateMenu();

	// THIS IS VERY TEMPORARY FOR EXAM, I DONT WANT TO WASTE TIME ON MAKING TRIGGERS 
	if (m_pCharacter->GetTransform()->GetWorldPosition().x > 25.5f) 
	{
		SceneManager::Get()->NextScene();
	}
}

void PortalScene::Draw()
{
	if (m_ScenePaused)
	{
		SpriteRenderer::Get()->AppendSprite(m_pMenuTexture, { 0,0 }, XMFLOAT4{ 1,1,1,0.5f }, {}, { m_SceneContext.windowWidth / 100, m_SceneContext.windowHeight / 100 }, 0, 0.2f);
	}
}

void PortalScene::UpdateMenu()
{
	// Initially false, it will keep the previous value of m_ScenePaused.
	static bool m_ScenePausedPrev = false;

	// force mouse to center of screen so when turning for long periods of time, the mouse wont move off screen
	m_SceneContext.pInput->ForceMouseToCenter(!m_ScenePaused);

	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, VK_ESCAPE))
		m_ScenePaused = !m_ScenePaused;

	// Detect a change in m_ScenePaused.
	if (m_ScenePaused != m_ScenePausedPrev)
	{
		// m_ScenePaused has changed, update m_ScenePausedPrev.
		m_ScenePausedPrev = m_ScenePaused;

		m_SceneContext.pInput->CursorVisible(m_ScenePaused);

		if (m_ScenePaused)
		{
			m_SceneContext.pGameTime->Stop();
			m_pMenuObject = AddChild(m_pMenuScene->GetMenuObject());
			m_pMenuScene->UpdateUI();
			m_pMenuScene->DrawUI();
		}
		else
		{
			RemoveChild(m_pMenuObject);
			m_pMenuScene->SetMenuObject(m_pMenuObject);
			m_SceneContext.pGameTime->Start();
		}

		// Set blur
		//m_pPostBlur->SetIsEnabled(m_ScenePaused);
	}


	if (m_ScenePaused)
	{
		// render a black transparent background during pause then 
		m_pMenuScene->UpdateUI();
		m_pMenuScene->DrawUI();
	}
}

void PortalScene::OnSceneActivated()
{
	m_SceneContext.settings.showInfoOverlay = true;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	m_pCharacter->GetCameraComponent()->SetActive(true);
	m_ScenePaused = false;

	m_SceneContext.pInput->CursorVisible(m_ScenePaused);

	SoundManager::Get()->ResumeChannelGroup(SoundManager::SoundChannel::Background);

	if (m_pCharacter->GetTransform()->GetWorldPosition().x > 25.5f)
	{
		SceneManager::Get()->NextScene();
	}
}

void PortalScene::OnSceneDeactivated()
{
	SoundManager::Get()->PauseChannelGroup(SoundManager::SoundChannel::Background);

	m_ScenePaused = false;
	UpdateMenu();

	if(!m_ScenePaused)
		m_SceneContext.pInput->CursorVisible(true);

}


