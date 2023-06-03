#pragma once
#include <array>
#include <filesystem>
#include <regex>

class Character;
class ModelComponent;
class PortalComponent;

class PortalTestScene : public GameScene
{
public:
	PortalTestScene() :GameScene(L"PortalTestScene") {}
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


	//std::wstring ConstructTextureFileName(const std::wstring& submeshName) const;
	void CreatePortals(CameraComponent* playerCamera);
	//void InitializePortal(Portal portal);
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
	std::array<GameObject*,2> m_pPortals{};

	GameObject* m_pMap;
};

