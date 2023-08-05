//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "PortalMainMenu.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/TorusPrefab.h"

PortalMainMenu::PortalMainMenu() :
	GameScene(L"PortalMainMenu")
{
}

GameObject* PortalMainMenu::GetMenuObject()
{
	RemoveChild(m_pMenuObject);
	return m_pMenuObject;
}

void PortalMainMenu::SetMenuObject(GameObject* pObject) 
{
	m_pMenuObject = AddChild(pObject);
}



void PortalMainMenu::Initialize()
{
	m_SceneContext.settings.showInfoOverlay = false;
	//m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = false;
	m_SceneContext.settings.clearColor = (XMFLOAT4)Colors::Black;
	
	// CAMERA 
	{
		m_pMenuCamera = new FixedCamera();
		AddChild(m_pMenuCamera);
		SetActiveCamera(m_pMenuCamera->GetComponent<CameraComponent>());
	}

	// GAME TITLE LOGO
	{
		GameLogoProps.pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/PortalFont_118px.fnt");
	}

	// UI MAIN MENU BUTTONS
	{
		MainUIProps.position.y = m_SceneContext.windowHeight / 2; // make sure that the buttons are always spawned from the center of the screen
		GameLogoProps.position.y = m_SceneContext.windowHeight / 2 - GameLogoProps.fontSize; // just like with the logo

		m_pMenuObject = new GameObject();
		AddChild(m_pMenuObject);

		for (size_t buttonIndex = 0; buttonIndex < MainUIProps.buttonCount; ++buttonIndex)
		{
			XMFLOAT4 boundingBox;
			XMFLOAT2 position{ MainUIProps.position.x, MainUIProps.position.y + (MainUIProps.size.y * buttonIndex) };

			boundingBox.x = position.x;
			boundingBox.y = position.y;
			boundingBox.z = MainUIProps.size.x;
			boundingBox.w = MainUIProps.size.y;

			UI_ButtonComponent* component = m_pMenuObject->AddComponent(new UI_ButtonComponent(MainUIProps.text[buttonIndex], MainUIProps.color, boundingBox));
			MainUIProps.buttonComponents.emplace_back(component);
		}
	}

	// NEW GAME UI
	{
		{
			auto pComponent = m_pMenuObject->AddComponent(new SpriteComponent(NewGameProps.assetPath, { 0.5f,0.5f }));
			m_pMenuObject->GetTransform()->Translate(m_SceneContext.windowWidth / 2, m_SceneContext.windowHeight / 2, 0);

			m_pSpriteComponents.emplace_back(pComponent);
		}

		// NEW GAME BUTTON START
		{
			XMFLOAT4 boundingBox;

			boundingBox.x = m_SceneContext.windowWidth / 2 + NewGameProps.buttonPos.x; // make sure that the buttons are always spawned from the center of the screen
			boundingBox.y = m_SceneContext.windowHeight / 2 - NewGameProps.buttonPos.y; // we do minus because the y axis is inverted
			boundingBox.z = NewGameProps.buttonSize1.x;
			boundingBox.w = NewGameProps.buttonSize1.y;

			UI_ButtonComponent* pComponent = m_pMenuObject->AddComponent(new UI_ButtonComponent(L"START", { 1,1,1,0 }, boundingBox));
			NewGameProps.buttonComponents.emplace_back(pComponent);
		}

		// NEW GAME BUTTON CANCEL
		{
			XMFLOAT4 boundingBox;

			boundingBox.x = m_SceneContext.windowWidth / 2 + NewGameProps.buttonPos.x; // make sure that the buttons are always spawned from the center of the screen
			boundingBox.y = m_SceneContext.windowHeight / 2 - NewGameProps.buttonPos.y; // we do minus because the y axis is inverted
			boundingBox.z = NewGameProps.buttonSize2.x;
			boundingBox.w = NewGameProps.buttonSize2.y;

			boundingBox.x += NewGameProps.buttonSize1.x + NewGameProps.padding; // since this is the second button, I keep the same position but just add an offset

			UI_ButtonComponent* pComponent = m_pMenuObject->AddComponent(new UI_ButtonComponent(L"CANCEL", { 1,1,1,0 }, boundingBox));
			NewGameProps.buttonComponents.emplace_back(pComponent);
		}

		// NEW GAME BUTTON PICTURE BUTTON
		{
			XMFLOAT4 boundingBox;

			boundingBox.x = m_SceneContext.windowWidth / 2 + NewGameProps.pictureButtonPos.x; // make sure that the buttons are always spawned from the center of the screen
			boundingBox.y = m_SceneContext.windowHeight / 2 - NewGameProps.pictureButtonPos.y; // we do minus because the y axis is inverted
			boundingBox.z = NewGameProps.pictureButtonSize.x;
			boundingBox.w = NewGameProps.pictureButtonSize.y;

			UI_ButtonComponent* pComponent = m_pMenuObject->AddComponent(new UI_ButtonComponent(L"PICTURE", { 1,1,1,0 }, boundingBox));
			NewGameProps.buttonComponents.emplace_back(pComponent);
		}
	}

	// OPTIONS UI
	{
		{
			auto pComponent = m_pMenuObject->AddComponent(new SpriteComponent(OptionsProps.assetPath, { 0.5f,0.5f }));
			m_pMenuObject->GetTransform()->Translate(m_SceneContext.windowWidth / 2, m_SceneContext.windowHeight / 2, 0);

			m_pSpriteComponents.emplace_back(pComponent);
		}

		// OPTIONS BUTTON OK
		{
			XMFLOAT4 boundingBox;

			boundingBox.x = m_SceneContext.windowWidth / 2 + OptionsProps.buttonPos.x; // make sure that the buttons are always spawned from the center of the screen
			boundingBox.y = m_SceneContext.windowHeight / 2 - OptionsProps.buttonPos.y; // we do minus because the y axis is inverted
			boundingBox.z = OptionsProps.buttonSize.x;
			boundingBox.w = OptionsProps.buttonSize.y;

			UI_ButtonComponent* pComponent = m_pMenuObject->AddComponent(new UI_ButtonComponent(L"OK", { 1,1,1,0 }, boundingBox));
			OptionsProps.buttonComponents.emplace_back(pComponent);
		}

		// OPTIONS BUTTON CANCEL
		{
			XMFLOAT4 boundingBox;

			boundingBox.x = m_SceneContext.windowWidth / 2 + OptionsProps.buttonPos.x; // make sure that the buttons are always spawned from the center of the screen
			boundingBox.y = m_SceneContext.windowHeight / 2 - OptionsProps.buttonPos.y; // we do minus because the y axis is inverted
			boundingBox.z = OptionsProps.buttonSize.x;
			boundingBox.w = OptionsProps.buttonSize.y;

			boundingBox.x += OptionsProps.buttonSize.x + OptionsProps.padding; // since this is the second button, I keep the same position but just add an offset

			UI_ButtonComponent* pComponent = m_pMenuObject->AddComponent(new UI_ButtonComponent(L"CANCEL", { 1,1,1,0 }, boundingBox));
			OptionsProps.buttonComponents.emplace_back(pComponent);
		}
	}
	// QUIT UI
	{
		// QUIT BOX
		{
			auto pComponent = m_pMenuObject->AddComponent(new SpriteComponent(QuitProps.assetPath, { 0.5f,0.5f }));
			m_pMenuObject->GetTransform()->Translate(m_SceneContext.windowWidth / 2, m_SceneContext.windowHeight / 2, 0);

			m_pSpriteComponents.emplace_back(pComponent);
		}

		// QUIT BUTTON OK
		{
			XMFLOAT4 boundingBox;

			boundingBox.x = m_SceneContext.windowWidth / 2 + QuitProps.buttonPos.x; // make sure that the buttons are always spawned from the center of the screen
			boundingBox.y = m_SceneContext.windowHeight / 2 - QuitProps.buttonPos.y; // we do minus because the y axis is inverted
			boundingBox.z = QuitProps.buttonSize1.x;
			boundingBox.w = QuitProps.buttonSize1.y;

			UI_ButtonComponent* pComponent = m_pMenuObject->AddComponent(new UI_ButtonComponent(L"QUIT GAME", { 1,1,1,0 }, boundingBox));
			QuitProps.buttonComponents.emplace_back(pComponent);
		}

		// QUIT BUTTON CANCEL
		{
			XMFLOAT4 boundingBox;

			boundingBox.x = m_SceneContext.windowWidth / 2 + QuitProps.buttonPos.x; // make sure that the buttons are always spawned from the center of the screen
			boundingBox.y = m_SceneContext.windowHeight / 2 - QuitProps.buttonPos.y; // we do minus because the y axis is inverted
			boundingBox.z = QuitProps.buttonSize2.x;
			boundingBox.w = QuitProps.buttonSize2.y;

			boundingBox.x += QuitProps.buttonSize1.x + QuitProps.padding; // since this is the second button, I keep the same position but just add an offset

			UI_ButtonComponent* pComponent = m_pMenuObject->AddComponent(new UI_ButtonComponent(L"CANCEL", { 1,1,1,0 }, boundingBox));
			QuitProps.buttonComponents.emplace_back(pComponent);
		}

	}

}

void PortalMainMenu::Update()
{
	UpdateUI();
}

void PortalMainMenu::Draw()
{
	DrawUI();
}

void PortalMainMenu::UpdateUI()
{

	std::vector<UI_ButtonComponent*> buttonComponentVec = m_pMenuObject->GetComponents<UI_ButtonComponent>();

		for (UI_ButtonComponent* pComponent : buttonComponentVec)
		{
			pComponent->SetEnabled(false);
		}

	switch (m_MenuState)
	{
		case PortalMainMenu::MenuState::Menu:
			UpdateMenu();
			break;
		case PortalMainMenu::MenuState::NewGame:
			UpdateNewGame();
			break;
		case PortalMainMenu::MenuState::Options:
			UpdateOptions();
			break;
		case PortalMainMenu::MenuState::Quit:
			UpdateQuit();
			break;
	}
}

void PortalMainMenu::DrawUI()
{
	//Optional
	switch (m_MenuState)
	{
		case PortalMainMenu::MenuState::Menu:
			DrawLogo();
			break;
		case PortalMainMenu::MenuState::NewGame:
			DrawNewGame();
			break;
		case PortalMainMenu::MenuState::Options:
			DrawOptions();
			break;
		case PortalMainMenu::MenuState::Quit:
			DrawQuit();
			break;
	}
}

void PortalMainMenu::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}

void PortalMainMenu::OnSceneActivated()
{
	MainUIProps.buttonComponents[0]->SetText(L"NEW GAME"); // when in the main menu, the name of the first button is "NEW GAME"
}

void PortalMainMenu::OnSceneDeactivated()
{
	MainUIProps.buttonComponents[0]->SetText(L"RESUME"); // when in different scenes, the name of the first button is "RESUME"
}

void PortalMainMenu::DrawLogo()
{
	// DRAW GAME TITLE LOGO
	{
		const auto& font = GameLogoProps.pFont;
		const auto& text = GameLogoProps.text;
		const auto& pos = GameLogoProps.position;
		const auto& color = GameLogoProps.color;

		TextRenderer::Get()->DrawText(font, text, pos, color);
	}
}

void PortalMainMenu::DrawNewGame()
{

}

void PortalMainMenu::DrawOptions()
{

}

void PortalMainMenu::DrawQuit()
{
}

void PortalMainMenu::UpdateMenu()
{
	for (UI_ButtonComponent* component : MainUIProps.buttonComponents)
	{
		component->SetEnabled(true);

		if (!component->GetClicked()) continue;

		if (component->GetText() == L"NEW GAME")
		{
			m_MenuState = MenuState::NewGame;
		}
		else if (component->GetText() == L"OPTIONS")
		{
			m_MenuState = MenuState::Options;
		}
		else if (component->GetText() == L"QUIT")
		{
			m_MenuState = MenuState::Quit;
		}
		else if (component->GetText() == L"RESUME")
		{

			SceneManager::Get()->SetActiveGameScene(L"PortalScene");
			SceneManager::Get()->GetActiveScene()->PauseScene(false);
		}
	}

	UpdateUIBox();
}

void PortalMainMenu::UpdateNewGame()
{
	for (UI_ButtonComponent* pComponent : NewGameProps.buttonComponents)
	{
		pComponent->SetEnabled(true);

		std::wstring* assetPath = &NewGameProps.assetPath;
		if (pComponent->GetHovering() && pComponent->GetText() == L"PICTURE")
			assetPath = &NewGameProps.assetPathHover;

		m_pSpriteComponents.at(static_cast<int>(MenuState::NewGame))->SetTexture(*assetPath);

		if (!pComponent->GetClicked()) continue;

		if (pComponent->GetText() == L"START" || pComponent->GetText() == L"PICTURE")
		{
			m_MenuState = MenuState::Menu;
			SceneManager::Get()->NextScene();
		}

		if (pComponent->GetText() == L"CANCEL")
		{
			m_MenuState = MenuState::Menu;
		}
	}
}

void PortalMainMenu::UpdateOptions()
{
	for (UI_ButtonComponent* pComponent : OptionsProps.buttonComponents)
	{
		pComponent->SetEnabled(true);

		if (!pComponent->GetClicked()) continue;

		if (pComponent->GetText() == L"OK")
		{
			m_MenuState = MenuState::Menu;
		}

		if (pComponent->GetText() == L"CANCEL")
		{
			m_MenuState = MenuState::Menu;
		}
	}
}

void PortalMainMenu::UpdateQuit()
{
	for (UI_ButtonComponent* pComponent : QuitProps.buttonComponents)
	{
		pComponent->SetEnabled(true);

		if (!pComponent->GetClicked()) continue;

		if (pComponent->GetText() == L"QUIT GAME")
		{
			PostQuitMessage(0);
		}

		if (pComponent->GetText() == L"CANCEL")
		{
			m_MenuState = MenuState::Menu;
		}
	}
}

// enables the UI box for the current menu state
void PortalMainMenu::UpdateUIBox()
{
	// first disable all UI boxes
	for (SpriteComponent* pComponent : m_pSpriteComponents)
	{
		pComponent->EnableRender(false);
	}

	if (m_MenuState == MenuState::Menu) return;


	int UIBoxIndex = static_cast<int>(m_MenuState);

	// enable the UI box for the current menu state
	m_pSpriteComponents.at(UIBoxIndex)->EnableRender(true);
}
