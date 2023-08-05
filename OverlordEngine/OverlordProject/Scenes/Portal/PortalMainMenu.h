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

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

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

	// all the magic numbers that have to do with positioning are just from checking them in photoshop with the original game
	struct GameLogoProperties
	{
		SpriteFont*				pFont{};
		const float 			fontSize = 118.f; // I cant change the fontsize in code because it is baked into a texture
		std::wstring			text{ L"Portal" };
		XMFLOAT2				position = { 110, 0 }; // vertical value will be set in initialize (it will be half of screen height)
		XMFLOAT4				color = XMFLOAT4{ Colors::WhiteSmoke };
	} GameLogoProps;

	struct ButtonProperties
	{
		SpriteFont*							pFont{};
		const float 						fontSize = 16.f; // I cant change the fontsize in code because it is baked into a texture
		int									buttonCount = 3;
		std::vector<std::wstring>			text{ L"NEW GAME", L"OPTIONS", L"QUIT" };
		XMFLOAT2							position = {115, 0 }; // this is the start position of the first button and all other buttons are vertically offset by height(), vertical value will be set in initialize (it will be half of screen height)
		XMFLOAT2 							size = { 134, 32 };
		XMFLOAT4							color = XMFLOAT4{ Colors::WhiteSmoke };

		std::vector<UI_ButtonComponent*>	buttonComponents;
	} ButtonProps;

	struct QuitProperties
	{
		GameObject* 				pQuitButton{};
		std::wstring				assetPath{ L"Textures/UI/QuitBox.tga" };

		//XMFLOAT2 			
		XMFLOAT2 					buttonPos = { -71, -10 }; // this is the position local to the center of the screen
		float						padding = 17; // this is the horizontal padding between the buttons
		XMFLOAT2					buttonSize1 = { 75, 24 };
		XMFLOAT2					buttonSize2 = { 64, 24 };

		std::vector<UI_ButtonComponent*>	buttonComponents;
	} QuitProps;

	struct OptionsProperties
	{
		GameObject*					pOptionsButton{};
		std::wstring				assetPath{ L"Textures/UI/OptionsBox.tga" };

		XMFLOAT2 					buttonPos = { 9, -168 }; // this is the position local to the center of the screen
		float						padding = 8; // this is the horizontal padding between the buttons
		XMFLOAT2					buttonSize = { 72, 24 };

		std::vector<UI_ButtonComponent*>	buttonComponents;


	} OptionsProps;

	struct NewGameProperties
	{
		GameObject*					pNewGameButton{};
		std::wstring				assetPath{ L"Textures/UI/NewGameBox.tga" };
		std::wstring				assetPathHover{ L"Textures/UI/NewgameBoxHover.tga" };

		//XMFLOAT2 			
		XMFLOAT2 					buttonPos = { 64, -102 }; // this is the position local to the center of the screen
		float						padding = 13; // this is the horizontal padding between the buttons
		XMFLOAT2					buttonSize1 = { 124, 24 };
		XMFLOAT2					buttonSize2 = { 72, 24 };
		XMFLOAT2                    pictureButtonPos{ -83, 80 };
		XMFLOAT2 					pictureButtonSize = { 168, 106 };

		
		std::vector<UI_ButtonComponent*>	buttonComponents;

	} NewGameProps;

	enum class MenuState
	{
		NewGame,
		Options,
		Quit,
		Menu
	} m_MenuState{ MenuState::Menu };

	GameObject* m_pMenuCamera{};
	GameObject* m_pMenuItem;

	std::vector<SpriteComponent*> m_pSpriteComponents;

	SpriteFont* m_pFont{};

	bool m_QuitPressed{ false };

};


