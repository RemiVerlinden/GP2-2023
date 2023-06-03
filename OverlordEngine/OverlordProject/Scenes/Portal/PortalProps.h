#pragma once
class ModelComponent;
class PhongMaterial;
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
	ModelComponent* pModelTop{};
	PhongMaterial* pPhong{};
	ButtonAnimComponent* m_pButtonAnim;

	GameObject* m_pCube;

	char** m_ClipNames{};
	UINT m_ClipCount{};
};

