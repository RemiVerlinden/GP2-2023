#include "stdafx.h"
#include "PGunReleaseState.h"
#include "PGunStateIncludes.h"
#include "../OverlordProject/SceneInputDefines/PortalInput.h"

PGunReleaseState::PGunReleaseState(GameObject* pGrabbedObject)
{
	auto pPickupComp = pGrabbedObject->GetComponent<PickUpObjectComponent>();
	if (pPickupComp)
		pPickupComp->SetGrabbed(false);
	else
		Logger::LogError(L"PGunReleaseState::Update() > No PickupObjectComponent found on grabbed object");
}

void PGunReleaseState::Enter(PSC* statecomponent)
{
	statecomponent->GetAnimationComponent()->SetAnimation(PortalgunAnimComponent::Release,true);
}

void PGunReleaseState::Update(PSC* statecomponent, const SceneContext& /*scenecontext*/)
{
	PortalgunAnimComponent* pAnim = statecomponent->GetAnimationComponent();
	if (pAnim->IsAnimationFinished())
	{

		statecomponent->SwitchState(std::make_unique<PGunIdleState>());
	}
}
