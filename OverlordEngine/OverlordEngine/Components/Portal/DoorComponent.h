#pragma once
#include "../OverlordProject/Portal/MapLoader.h"
class PhongMaterial_Skinned;
class GameObject;
class DoorComponent final : public BaseComponent
{
public:
	DoorComponent() = default;
	~DoorComponent() override = default;

	void StartInteraction();
	void EndInteraction();
protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;

private:
	GameObject* CreateDoorSide(bool front);
	void SetDoorState(bool open);
	void SetDoorCollision(bool enable);


	PhongMaterial_Skinned* m_pDoormaterial = nullptr;
	bool m_IsOpen = false;
	std::pair<GameObject*, GameObject*> m_pDoorSides;

	enum class SoundLibrary
	{
		Open,
		Close
	};

	static inline std::unordered_map<SoundLibrary, FMOD::Sound*> m_pSounds{};

};
