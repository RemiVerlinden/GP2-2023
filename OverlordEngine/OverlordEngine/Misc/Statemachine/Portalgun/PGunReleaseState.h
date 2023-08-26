#pragma once
#include "PortalgunState.h"

class PGunReleaseState final : public PortalgunState
{
public:
	PGunReleaseState(GameObject* pGrabbedObject);
	virtual ~PGunReleaseState() = default;

	void Enter(PSC* statecomponent) override;
	void Update(PSC* statecomponent, const SceneContext& scenecontext) override;
private:
};