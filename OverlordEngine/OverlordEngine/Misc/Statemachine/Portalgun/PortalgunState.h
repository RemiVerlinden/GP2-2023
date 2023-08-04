#pragma once
class PortalgunStatemachineComponent;
#include "Components\Portal\PortalgunStatemachineComponent.h" // also includes PortalgunAnimComponent

class PortalgunState
{
public:
	using PSC = PortalgunStatemachineComponent;

	virtual ~PortalgunState() {};
	virtual void Enter(PSC* context) = 0;
	virtual void Update(PSC* context, const SceneContext& scenecontext) = 0;

private:

};