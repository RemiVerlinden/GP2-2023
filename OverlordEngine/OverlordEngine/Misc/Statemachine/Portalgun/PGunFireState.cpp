#include "stdafx.h"
#include "PGunStateIncludes.h"
#include "../OverlordProject/SceneInputDefines/PortalInput.h"


void PGunFireState::Enter(PSC* statecomponent)
{
	statecomponent->GetAnimationComponent()->SetAnimation(PortalgunAnimComponent::Fire, true);
}

void PGunFireState::Update(PSC* statecomponent, const SceneContext& /*scenecontext*/)
{
	PortalgunAnimComponent* pAnim = statecomponent->GetAnimationComponent();
	if (pAnim->IsAnimationFinished())
	{
		statecomponent->SwitchState(std::make_unique<PGunIdleState>());
	}
}
