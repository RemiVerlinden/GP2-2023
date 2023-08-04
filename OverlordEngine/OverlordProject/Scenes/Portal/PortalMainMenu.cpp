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

void PortalMainMenu::Initialize()
{
	m_SceneContext.settings.showInfoOverlay = false;
	//m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = false;

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

	// FIRST UI ELEMENT
	{
		m_pMenuItem = new GameObject();
		AddChild(m_pMenuItem);
		//m_pMenuItem->AddComponent(new UI_ButtonComponent(L"PORTAL", XMFLOAT4{ DirectX::Colors::White }, { 0.06f,0.43f,0.07f,0.03f }));
		m_pMenuItem->AddComponent(new UI_ButtonComponent(L"NEW GAME", XMFLOAT4{ DirectX::Colors::White }, { 0.06f,0.5f,0.07f,0.03f }));
		m_pMenuItem->AddComponent(new UI_ButtonComponent(L"OPTIONS", XMFLOAT4{ DirectX::Colors::White }, { 0.06f,0.53f,0.07f,0.03f }));
		m_pMenuItem->AddComponent(new UI_ButtonComponent(L"QUIT", XMFLOAT4{ DirectX::Colors::White }, { 0.06f,0.56f,0.07f,0.03f }));
	}



}

void PortalMainMenu::Update()
{
	{
		const auto& font	= GameLogoProps.pFont;
		const auto& text	= GameLogoProps.text;
		const auto& pos		= GameLogoProps.position;
		const auto& color	= GameLogoProps.color;

		TextRenderer::Get()->DrawText(font, text, pos, color);
	}
}

void PortalMainMenu::Draw()
{
	//Optional
}

void PortalMainMenu::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}
