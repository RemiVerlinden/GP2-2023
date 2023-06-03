#pragma once
class ButtonAnimComponent : public BaseComponent
{
public:
	ButtonAnimComponent(GameObject* button);
	~ButtonAnimComponent() override = default;

	void SetPressed(bool pressed);

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;

private:
	GameObject* m_pButton;

	bool m_IsPressed = false;
	float buttonState = 0; // this is in percentage float [0-1] -> [0-100%] on how much the button is pressed
	XMFLOAT3 m_OriginalPosition;

	struct ButtonAnimationInfo 
	{
		float pressDepth = 1.f;
		float animationTime = 1.f; // in seconds
	};

	ButtonAnimationInfo m_AnimInfo;
};
