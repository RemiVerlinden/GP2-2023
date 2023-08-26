#pragma once
#include "PortalgunState.h"

class PGunPickUpState final : public PortalgunState
{
public:
	PGunPickUpState(GameObject* pGrabbedObject);
	virtual ~PGunPickUpState() = default;

	void Enter(PSC* statecomponent) override;
	void Update(PSC* statecomponent, const SceneContext& scenecontext) override;
private:

	PickUpObjectComponent* m_pPickUpComponent = nullptr;
};