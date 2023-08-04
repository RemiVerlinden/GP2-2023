#include "stdafx.h"
#include "PGunStateIncludes.h"
#include "../OverlordProject/SceneInputDefines/PortalInput.h"

void PGunIdleState::Enter(PSC* statecomponent)
{
	statecomponent->GetAnimationComponent()->SetAnimation(PortalgunAnimComponent::Idle);
}

void PGunIdleState::Update(PSC* statecomponent, const SceneContext& scenecontext)
{
	bool fire	= scenecontext.pInput->IsActionTriggered(Input::FirePrimary);
	fire	   |= scenecontext.pInput->IsActionTriggered(Input::FireSecondary);
	if (fire)
	{
		statecomponent->SwitchState(std::make_unique<PGunFireState>());
	}
}
