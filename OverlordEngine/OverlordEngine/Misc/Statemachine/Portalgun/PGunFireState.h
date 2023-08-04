#pragma once
#include "PortalgunState.h"

class PGunFireState final : public PortalgunState
{
public:
	virtual ~PGunFireState() = default;

	void Enter(PSC* statecomponent) override;
	void Update(PSC* statecomponent, const SceneContext& scenecontext) override;
private:

};