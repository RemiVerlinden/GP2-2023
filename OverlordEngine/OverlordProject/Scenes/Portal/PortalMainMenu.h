#pragma once
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

	struct GameLogoProperties
	{
		SpriteFont*				pFont{};
		std::wstring			text{ L"Portal" };
		XMFLOAT2				position = { 110, 400 };
		XMFLOAT4				color = XMFLOAT4{ Colors::WhiteSmoke };
	} GameLogoProps;

	GameObject* m_pMenuCamera{};
	GameObject* m_pMenuItem;

	SpriteFont* m_pFont{};

};


