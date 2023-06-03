#pragma once
class ButtonAnimComponent : public BaseComponent
{
public:
	ButtonAnimComponent();
	~ButtonAnimComponent() override = default;

	void SetPressed(bool pressed);
	bool GetPressed() { return m_IsPressed; }
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
};
