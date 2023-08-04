#include "stdafx.h"
#include "PortalgunStatemachineComponent.h"
#include "Misc\Statemachine\Portalgun\PortalgunState.h"
#include "Misc\Statemachine\Portalgun\PGunIdleState.h"

PortalgunStatemachineComponent::PortalgunStatemachineComponent(PortalgunAnimComponent* animComponent)
{
	m_pPortalgunAnimComponent = animComponent;
}

void PortalgunStatemachineComponent::SwitchState(std::unique_ptr<PortalgunState>&& state)
{
	m_pAnimationState = std::move(state);
	m_pAnimationState->Enter(this);
}

void PortalgunStatemachineComponent::Initialize(const SceneContext&)
{
	SwitchState(std::make_unique<PGunIdleState>());
}

void PortalgunStatemachineComponent::Update(const SceneContext& context)
{
	m_pAnimationState->Update(this, context);
}
