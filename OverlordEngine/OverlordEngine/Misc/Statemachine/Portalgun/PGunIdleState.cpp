#include "stdafx.h"
#include "PGunStateIncludes.h"
#include "../OverlordProject/SceneInputDefines/PortalInput.h"
#include "../OverlordProject/Scenes/Portal/PortalScene.h"

void PGunIdleState::Enter(PSC* statecomponent)
{
	statecomponent->GetAnimationComponent()->SetAnimation(PortalgunAnimComponent::Idle);
}

void PGunIdleState::Update(PSC* statecomponent, const SceneContext& scenecontext)
{
	// if we are in the menu, we dont want to check for input
	if(!scenecontext.pGameTime->IsRunning()) return;


	PortalScene::Portal type = PortalScene::Portal::None;
	if(scenecontext.pInput->IsActionTriggered(Input::FirePrimary))
		type = PortalScene::Portal::Blue;
	else if(scenecontext.pInput->IsActionTriggered(Input::FireSecondary))
		type = PortalScene::Portal::Orange;

	if (type != PortalScene::Portal::None)
	{
		statecomponent->SwitchState(std::make_unique<PGunFireState>(type));
	}
}
