#pragma once
#include <array>
class Character;

class CharacterScene : public GameScene
{
public:
	CharacterScene() :GameScene(L"CharacterScene") {}
	~CharacterScene() override = default;
	CharacterScene(const CharacterScene& other) = delete;
	CharacterScene(CharacterScene&& other) noexcept = delete;
	CharacterScene& operator=(const CharacterScene& other) = delete;
	CharacterScene& operator=(CharacterScene&& other) noexcept = delete;

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

	void InitializePortal(Portal portal);
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

};

