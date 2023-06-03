#pragma once
class TestPortalAnimations: public GameScene
{
public:
	TestPortalAnimations() : GameScene(L"TestPortalAnimations") {}
	~TestPortalAnimations() override;
	TestPortalAnimations(const TestPortalAnimations& other) = delete;
	TestPortalAnimations(TestPortalAnimations&& other) noexcept = delete;
	TestPortalAnimations& operator=(const TestPortalAnimations& other) = delete;
	TestPortalAnimations& operator=(TestPortalAnimations&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;

private:
	ModelAnimator* pAnimator{};

	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 1.f };

	char** m_ClipNames{};
	UINT m_ClipCount{};
};

