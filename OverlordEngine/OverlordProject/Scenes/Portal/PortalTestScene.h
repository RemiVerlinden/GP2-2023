#pragma once
#include <array>
#include <filesystem>
#include <regex>

class Character;
class ModelComponent;
class PortalComponent;
class PostColorGrading;
class PostSSAO;
class PortalTestScene : public GameScene
{
public:
	PortalTestScene() :GameScene(L"Chamber 02") {}
	~PortalTestScene() override = default;
	PortalTestScene(const PortalTestScene& other) = delete;
	PortalTestScene(PortalTestScene&& other) noexcept = delete;
	PortalTestScene& operator=(const PortalTestScene& other) = delete;
	PortalTestScene& operator=(PortalTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;

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


	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump
	};

	Character* m_pCharacter{};
	GameObject* m_pPlayerAnimObject{};
	std::array<GameObject*,2> m_pPortals{};

	GameObject* m_pCube{};
	DoorComponent* m_pDoor{};

	GameObject* m_pMap;
};

