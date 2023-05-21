#pragma once
#include <array>
#include <filesystem>
#include <regex>

class Character;
class ModelComponent;

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
		Orange,
		Blue,
		PortalsCount
	};

	void LoadMap();
	void LoadMapTextures(const std::wstring& mapname, ModelComponent* pMapModel);
	std::filesystem::path SearchForMatchingFile(fs::path directory, std::wregex pattern);

	//std::wstring ConstructTextureFileName(const std::wstring& submeshName) const;
	void CreatePortals();
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
	GameObject* pBall1;
	GameObject* pBall2;

	GameObject* m_pMap;
};

