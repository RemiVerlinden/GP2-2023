#pragma once
#include "PortalgunState.h"
#include "../OverlordProject/Scenes/Portal/PortalScene.h"

class PGunFireState final : public PortalgunState
{
public:
	PGunFireState(PortalScene::Portal type);
	virtual ~PGunFireState() = default;

	void Enter(PSC* statecomponent) override;
	void Update(PSC* statecomponent, const SceneContext& scenecontext) override;


private:
	PortalScene::Portal m_FireType;
};