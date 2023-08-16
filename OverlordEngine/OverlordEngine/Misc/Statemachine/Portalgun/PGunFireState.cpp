#include "stdafx.h"
#include "PGunStateIncludes.h"
#include "../OverlordProject/SceneInputDefines/PortalInput.h"
#include "../OverlordProject/Materials/Portal/Weapon/PortalGun_Glow_Material.h"

PGunFireState::PGunFireState(PortalScene::Portal type)
{
	m_FireType = type;
}

void PGunFireState::Enter(PSC* statecomponent)
{
	PortalScene::FirePortalInUpdate(m_FireType);
	PortalGun_Glow_Material::SetGlowColor(m_FireType);
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
