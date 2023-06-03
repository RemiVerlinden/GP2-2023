#pragma once
class PortalProps: public GameScene
{
public:
	PortalProps() : GameScene(L"PortalProps") {}
	~PortalProps() override = default;
	PortalProps(const PortalProps& other) = delete;
	PortalProps(PortalProps&& other) noexcept = delete;
	PortalProps& operator=(const PortalProps& other) = delete;
	PortalProps& operator=(PortalProps&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;
private:
	ModelComponent* pModel{};
	ButtonAnimComponent* m_pButtonAnim;
	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 1.f };

	char** m_ClipNames{};
	UINT m_ClipCount{};
};

