#include "stdafx.h"
#include "ButtonAnimComponent.h"
ButtonAnimComponent::ButtonAnimComponent()
	: m_IsPressed(false)
	, m_AnimInfo({0.8f,0.35f})
{ 
}

void ButtonAnimComponent::Initialize(const SceneContext&)
{
	m_OriginalPosition = m_pGameObject->GetTransform()->GetPosition();
}

void ButtonAnimComponent::SetPressed(bool pressed)
{
	m_IsPressed = pressed;
}

void ButtonAnimComponent::Update(const SceneContext& context)
{
	float timestep = context.pGameTime->GetElapsed();

	if (m_IsPressed && buttonState < 1.f)
	{
			buttonState += timestep / m_AnimInfo.animationTime;
	}
	else if(!m_IsPressed && buttonState > 0.f)
	{
			buttonState -= timestep / m_AnimInfo.animationTime;
	}

	if (buttonState <= 0.f || 1.f <= buttonState) return;


	XMFLOAT3 pressPosition = m_OriginalPosition;
	pressPosition.y -= buttonState * m_AnimInfo.pressDepth;

	GameObject* pButton = m_pGameObject; // just for clarity
	pButton->GetTransform()->Translate(pressPosition);
}




