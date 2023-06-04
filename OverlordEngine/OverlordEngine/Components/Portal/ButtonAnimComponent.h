#pragma once
#include <variant>
#include "Components\Portal\DoorComponent.h"

class ButtonAnimComponent final : public BaseComponent
{
public:
	using InteractionComponent = std::variant<DoorComponent*>; // all components that can interact with the button

	ButtonAnimComponent();
	~ButtonAnimComponent() override = default;

	void SetPressed(bool pressed);
	bool GetPressed() { return m_IsPressed; }
	void AddInteractionComponent(InteractionComponent interactionComponent);
protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;

private:

	bool m_IsPressed = false;
	float buttonState = 0; // this is in percentage float [0-1] -> [0-100%] on how much the button is pressed
	XMFLOAT3 m_OriginalPosition;

	struct ButtonAnimationInfo
	{
		float pressDepth = 1.f;
		float animationTime = 1.f; // in seconds
	};

	ButtonAnimationInfo m_AnimInfo;

	std::vector<InteractionComponent> m_InterationComponents{};
};
