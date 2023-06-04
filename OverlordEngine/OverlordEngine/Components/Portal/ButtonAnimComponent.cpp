#include "stdafx.h"
#include "ButtonAnimComponent.h"
ButtonAnimComponent::ButtonAnimComponent()
	: m_IsPressed(false)
	, m_AnimInfo({ 0.3f,0.35f })
{
}

void ButtonAnimComponent::Initialize(const SceneContext&)
{
}

void ButtonAnimComponent::SetPressed(bool pressed)
{
	m_IsPressed = pressed;
	for (InteractionComponent component : m_InterationComponents)
	{
		std::visit([pressed](auto& c)
		{
			if (pressed)
				c->StartInteraction();
			else
				c->EndInteraction();
		}, component);

	}
}

void ButtonAnimComponent::AddInteractionComponent(InteractionComponent interactionComponent)
{
	m_InterationComponents.push_back(interactionComponent);
}

void ButtonAnimComponent::Update(const SceneContext& context)
{
	float timestep = context.pGameTime->GetElapsed();

	if (m_IsPressed && buttonState < 1.f)
	{
		buttonState += timestep / m_AnimInfo.animationTime;
	}
	else if (!m_IsPressed && buttonState > 0.f)
	{
		buttonState -= timestep / m_AnimInfo.animationTime;
	}

	if (buttonState <= 0.f || 1.f <= buttonState) return;

	static XMFLOAT3 originalPosition = m_pGameObject->GetTransform()->GetPosition();

	XMFLOAT3 pressPosition = originalPosition;
	pressPosition.y -= buttonState * m_AnimInfo.pressDepth;

	GameObject* pButton = m_pGameObject; // just for clarity
	pButton->GetTransform()->Translate(pressPosition);
	auto rigidBodies = pButton->GetComponents<RigidBodyComponent>();

	// I have 2 rigidbodies attached to my button and for some reason only a single collider moves locally with the button
	// so I have to manually set the position of the other one
	for (RigidBodyComponent* component : rigidBodies)
	{
		if (!component->IsKinematic()) continue;
		auto pRigidActor = component->GetPxRigidActor();
		pRigidActor->setGlobalPose(physx::PxTransform({ pressPosition.x,pressPosition.y,pressPosition.z }));
	}
}




