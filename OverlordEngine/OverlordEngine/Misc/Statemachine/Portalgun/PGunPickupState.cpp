#include "stdafx.h"
#include "PGunPickupState.h"
#include "PGunStateIncludes.h"
#include "../OverlordProject/SceneInputDefines/PortalInput.h"

PGunPickUpState::PGunPickUpState(GameObject* pGrabbedObject)
{
	m_pPickUpComponent = pGrabbedObject->GetComponent<PickUpObjectComponent>();
	if (m_pPickUpComponent)
		m_pPickUpComponent->SetGrabbed(true);
	else
		Logger::LogError(L"PGunReleaseState::Update() > No PickupObjectComponent found on grabbed object");
}

void PGunPickUpState::Enter(PSC* statecomponent)
{
	statecomponent->GetAnimationComponent()->SetAnimation(PortalgunAnimComponent::Pickup, true);
}

void PGunPickUpState::Update(PSC* statecomponent, const SceneContext& scenecontext)
{
	if (scenecontext.pInput->IsActionTriggered(Input::Use) || !m_pPickUpComponent->IsGrabbed())
	{
		statecomponent->SwitchState(std::make_unique<PGunReleaseState>(m_pPickUpComponent->GetGameObject()));
	}
}
