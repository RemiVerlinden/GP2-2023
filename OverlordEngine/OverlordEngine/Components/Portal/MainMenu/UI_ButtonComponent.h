#pragma once

// NOTE ALL DIMENSIONS ARE STORED IN PERCENTAGES AND THEN CONVERTED TO PIXELS WITH SCENECONTEXT::WINDOWWIDTH - WINDOWHEIGHT

class GameObject;
class SpriteFont;
class UI_ButtonComponent final : public BaseComponent
{
public:
	UI_ButtonComponent(const std::wstring& text, const XMFLOAT4& m_TextColor, const XMFLOAT4& boundingBox);
	~UI_ButtonComponent() = default;

	// is the mouse hovering over the button
	bool GetClicked() const;
	// has the mouse clicked on the button
	bool GetHovering() const { return m_Hovering; }
	const std::wstring& GetText() const { return m_Text; }
	void SetText(const std::wstring& text) { m_Text = text; }

	void SetEnabled(bool enabled) { m_Enabled = enabled; }

private:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext&) override; //update_W9

	void DrawButtonText() const;
	void SoundOnHoverStart();
	void Play2DSound(FMOD::Sound* pSound);

	XMFLOAT2 PixelToPercent(const SceneContext& context, const POINT& pos) const;
	XMFLOAT2 PixelToPercent(const SceneContext& context, const XMFLOAT2& pos) const;
	XMFLOAT2 PercentToPixel(const SceneContext& context, const POINT& pos) const;
	XMFLOAT2 PercentToPixel(const SceneContext& context, const XMFLOAT2& pos) const;

	bool m_Enabled = false;

	std::wstring m_Text;
	XMFLOAT4 m_TextColor;
	SpriteFont* m_pFont{};
	float m_FontSize{ };

	XMFLOAT4 m_BoundingBox; // X, Y, WIDTH, HEIGHT

	bool m_Hovering{}; // is cursor hovering over button
	bool m_HoveringPreviousFrame{}; // was cursor hovering over button previous frame
	bool m_Clicked{}; // is button clicked

	// SOUND
	FMOD::Channel* m_pChannel2D{};

	FMOD::Sound* m_pHoverSound;
	FMOD::Sound* m_pClickSound;
};