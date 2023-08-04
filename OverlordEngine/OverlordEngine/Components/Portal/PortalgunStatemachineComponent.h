#pragma once
#include "PortalgunAnimComponent.h"

class GameObject;
class PortalgunState;
class PortalgunStatemachineComponent final : public BaseComponent
{
public:
	PortalgunStatemachineComponent(PortalgunAnimComponent* animComponent);
	~PortalgunStatemachineComponent() override = default;

	PortalgunAnimComponent* GetAnimationComponent() const { return m_pPortalgunAnimComponent; }
	void SwitchState(std::unique_ptr<PortalgunState>&& state);

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;

private:
	PortalgunAnimComponent* m_pPortalgunAnimComponent = nullptr;
	std::unique_ptr<PortalgunState> m_pAnimationState;
};
