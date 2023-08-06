#pragma once
class SpriteComponent;
class PortalMainMenu final : public GameScene
{
public:
	PortalMainMenu();
	~PortalMainMenu() override = default;

	PortalMainMenu(const PortalMainMenu& other) = delete;
	PortalMainMenu(PortalMainMenu&& other) noexcept = delete;
	PortalMainMenu& operator=(const PortalMainMenu& other) = delete;
	PortalMainMenu& operator=(PortalMainMenu&& other) noexcept = delete;

	GameObject* GetMenuObject();
	void SetMenuObject(GameObject* pObject);
	void UpdateUI();
	void DrawUI();

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;
	virtual void OnSceneActivated() override;
	virtual void OnSceneDeactivated() override;
private:
	void DrawLogo();
	void DrawNewGame();
	void DrawOptions();
	void DrawQuit();

	void UpdateMenu();
	void UpdateNewGame();
	void UpdateOptions();
	void UpdateQuit();
	void UpdateUIBox();
	void InitializeLevelMeshes();



	//----------------------------------------------------------------------------------------------

	// all the magic numbers that have to do with positioning are just from checking them in photoshop with the original game

	struct UIProperties // this is the base of the start menu UI (logo, new game, options, quit)
	{
		SpriteFont* pFont{};
		float fontSize;		 
		XMFLOAT2 position;
		XMFLOAT4 color;
	};

	struct GameLogoProperties : public UIProperties
	{
		GameLogoProperties()
		{
			fontSize = 118.f; // I cant change the fontsize in code because it is baked into a texture
			text = L"Portal";
			position = { 110, 0 }; // vertical value will be set in initialize (it will be half of screen height)
			color = XMFLOAT4{ Colors::WhiteSmoke };
		}

		std::wstring text;
	} GameLogoProps;

	struct MainUIProperties : public UIProperties
	{
		MainUIProperties()
		{
			fontSize = 16.f; // I cant change the fontsize in code because it is baked into a texture
			buttonCount = 3;
			text = { L"NEW GAME", L"OPTIONS", L"QUIT" };
			position = { 115, 0 }; // this is the start position of the first button and all other buttons are vertically offset by height(), vertical value will be set in initialize (it will be half of screen height)
			size = { 134, 32 };
			color = XMFLOAT4{ Colors::WhiteSmoke };
		}

		int buttonCount;
		std::vector<std::wstring> text;
		XMFLOAT2 size;
		std::vector<UI_ButtonComponent*> buttonComponents;
	} MainUIProps;


	//----------------------------------------------------------------------------------------------


	struct ButtonBaseProperties
	{
		std::wstring assetPath;
		XMFLOAT2 buttonPos;									// this is the position local to the center of the screen
		float padding;										// this is the horizontal padding between the buttons
		std::vector<UI_ButtonComponent*> buttonComponents;
	};

	struct QuitProperties : ButtonBaseProperties
	{
		QuitProperties()
		{
			assetPath = L"Textures/UI/QuitBox.tga";
			buttonPos = { -71, -10 };
			padding = 17;
		}

		XMFLOAT2 buttonSize1 = { 75, 24 };
		XMFLOAT2 buttonSize2 = { 64, 24 };
	} QuitProps;

	struct OptionsProperties : ButtonBaseProperties
	{
		OptionsProperties()
		{
			assetPath = L"Textures/UI/OptionsBox.tga";
			buttonPos = { 9, -168 };
			padding = 8;
		}

		XMFLOAT2 buttonSize = { 72, 24 };
	} OptionsProps;

	struct NewGameProperties : ButtonBaseProperties
	{
		NewGameProperties()
		{
			assetPath = L"Textures/UI/NewGameBox.tga";
			assetPathHover = L"Textures/UI/NewgameBoxHover.tga";
			buttonPos = { 64, -102 };
			padding = 13;
		}

		std::wstring assetPathHover;
		XMFLOAT2 buttonSize1 = { 124, 24 };
		XMFLOAT2 buttonSize2 = { 72, 24 };
		XMFLOAT2 pictureButtonPos{ -83, 80 };
		XMFLOAT2 pictureButtonSize = { 168, 106 };
	} NewGameProps;


	//----------------------------------------------------------------------------------------------


	enum class MenuState
	{
		NewGame,
		Options,
		Quit,
		Menu
	} m_MenuState{ MenuState::Menu };

	GameObject* m_pMenuCamera{};
	GameObject* m_pMenuObject{};

	std::vector<SpriteComponent*> m_pSpriteComponents;

	SpriteFont* m_pFont{};

	bool m_QuitPressed{ false };

	XMFLOAT3 m_Translate{ -22.43f, 9.37f,  -2.00f };
	XMFLOAT3 m_StartRotation{ 17.80f, 122.90f, 3.0f };
};


