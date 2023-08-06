#include "stdafx.h"
#include "UI_ButtonComponent.h"

UI_ButtonComponent::UI_ButtonComponent(const std::wstring& text, const XMFLOAT4& m_TextColor, const XMFLOAT4& boundingBox)
	:m_Text(text)
	, m_TextColor(m_TextColor)
	, m_BoundingBox(boundingBox)
{
	m_enablePostDraw = true;
}

void UI_ButtonComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/PortalFont_16px.fnt");
	m_FontSize = 16.f;

	// INITIALIZE SOUND
	{
		// HOVER SOUND
		auto pFmod = SoundManager::Get()->GetSystem();

		FMOD_RESULT result = pFmod->createStream("Resources/Sounds/buttonrollover.wav", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pHoverSound);
		SoundManager::Get()->ErrorCheck(result);

		result = pFmod->playSound(m_pHoverSound, nullptr, true, &m_pChannel2D);
		SoundManager::Get()->ErrorCheck(result);


		// CLICK SOUND
		result = pFmod->createStream("Resources/Sounds/buttonclickrelease.wav", FMOD_DEFAULT | FMOD_LOOP_OFF, nullptr, &m_pClickSound);
		SoundManager::Get()->ErrorCheck(result);

		result = pFmod->playSound(m_pClickSound, nullptr, true, &m_pChannel2D);
		SoundManager::Get()->ErrorCheck(result);




		m_pChannel2D->setVolume(1.f);
	}
}
bool UI_ButtonComponent::GetClicked() const
{
	return m_Enabled ? m_Clicked : false;
}

void UI_ButtonComponent::Update(const SceneContext& /*context*/)
{
	if (!m_Enabled) return;

	// ease of use ----------------------------------------------
	XMFLOAT2 position{ m_BoundingBox.x, m_BoundingBox.y };
	float width = m_BoundingBox.z;
	float height = m_BoundingBox.w;
	//-----------------------------------------------------------

	m_Hovering = false; // reset
	m_Clicked = false;

	const auto& mousePos = InputManager::GetMousePosition();

	if (mousePos.x >= position.x && mousePos.x <= position.x + width &&
		mousePos.y >= position.y && mousePos.y <= position.y + height)
	{
		m_Hovering = true;
		if (InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON))
		{
			m_Clicked = true;
			PlayFmodSound(m_pClickSound);
		}
	}

	DrawButtonText();

	SoundOnHoverStart();
}

void UI_ButtonComponent::DrawButtonText() const
{
	XMFLOAT2 position = XMFLOAT2{ m_BoundingBox.x, m_BoundingBox.y };
	position.y += m_FontSize / 2; // make sure the font is drawn in the center of the boundingbox
	

	XMFLOAT4 color = m_TextColor;
	if (m_Hovering && color.w != 0.f) // if the alpha is 0, dont bother changing it
		color.w = 0.7f;

	TextRenderer::Get()->DrawText(m_pFont, m_Text, position, color);

}

void UI_ButtonComponent::Draw(const SceneContext& /*context*/)
{
	if (!m_Enabled) return;

}

void UI_ButtonComponent::SoundOnHoverStart()
{
	if (m_Hovering && !m_HoveringPreviousFrame)
	{
		m_HoveringPreviousFrame = true;

		PlayFmodSound(m_pHoverSound);
	}
	else if(!m_Hovering)
		m_HoveringPreviousFrame = false;
}

void UI_ButtonComponent::PlayFmodSound(FMOD::Sound* pSound)
{
	const auto pFmod{ SoundManager::Get()->GetSystem() };

	FMOD_RESULT result = pFmod->playSound(pSound, nullptr, false, &m_pChannel2D);
	SoundManager::Get()->ErrorCheck(result);
}




//=================================================================================================




XMFLOAT2 UI_ButtonComponent::PixelToPercent(const SceneContext& context, const POINT& pos)const
{
	return  XMFLOAT2{ pos.x / context.windowWidth, pos.y / context.windowHeight };
}

XMFLOAT2 UI_ButtonComponent::PixelToPercent(const SceneContext& context, const XMFLOAT2& pos)const
{
	return XMFLOAT2{ pos.x / context.windowWidth, pos.y / context.windowHeight };
}

XMFLOAT2 UI_ButtonComponent::PercentToPixel(const SceneContext& context, const POINT& pos)const
{
	return XMFLOAT2{ pos.x * context.windowWidth, pos.y * context.windowHeight };
}

XMFLOAT2 UI_ButtonComponent::PercentToPixel(const SceneContext& context, const XMFLOAT2& pos)const
{
	return XMFLOAT2{ pos.x * context.windowWidth, pos.y * context.windowHeight };
}


