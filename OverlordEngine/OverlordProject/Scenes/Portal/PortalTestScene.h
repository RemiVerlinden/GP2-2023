#pragma once
#include <array>
#include <filesystem>
#include <regex>

class Character;
class ModelComponent;
class PortalComponent;
class PostColorGrading;
class PostSSAO;
class PortalMainMenu;
class PostBlur;
class PortalTestScene : public GameScene
{
public:
	PortalTestScene() :GameScene(L"Chamber 02") {}
	~PortalTestScene() override;
	PortalTestScene(const PortalTestScene& other) = delete;
	PortalTestScene(PortalTestScene&& other) noexcept = delete;
	PortalTestScene& operator=(const PortalTestScene& other) = delete;
	PortalTestScene& operator=(PortalTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;
	void Draw() override;
	virtual void OnSceneActivated();

private:
	enum Portal
	{
		Blue,
		Orange,
		PortalsCount
	};

	friend PortalComponent;


	void CreatePortals(CameraComponent* playerCamera);
	void MovePortal(Portal portal);
	void UpdateMenu();
	Character* m_pCharacter{};
	GameObject* m_pPlayerAnimObject{};
	std::array<GameObject*,2> m_pPortals{};

	GameObject* m_pCube{};
	DoorComponent* m_pDoor{};

	GameObject* m_pMap;

	PortalMainMenu* m_pMenuScene{};
	GameObject* m_pMenuObject{};
	TextureData* m_pMenuTexture{};
	PostBlur* m_pPostBlur{};
};

