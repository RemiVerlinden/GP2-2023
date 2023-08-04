#pragma once
#include "PortalgunState.h"

class PGunIdleState final : public PortalgunState
{
public:
	virtual ~PGunIdleState() = default;

	void Enter(PSC* statecomponent) override;
	void Update(PSC* statecomponent, const SceneContext& scenecontext) override;
private:

};