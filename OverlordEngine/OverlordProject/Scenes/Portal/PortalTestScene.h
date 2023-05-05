#pragma once
#include <array>
class Character;

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
};

