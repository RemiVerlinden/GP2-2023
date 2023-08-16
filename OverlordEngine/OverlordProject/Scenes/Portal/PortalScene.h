#pragma once
#include <array>
#include <filesystem>
#include <regex>

class Character;
class ModelComponent;
class PortalComponent;
class PostColorGrading;
class PostChromatic;
class PortalMainMenu;
class PostBlur;
class CrosshairComponent;
class PortalScene : public GameScene
{
public:
	PortalScene() :GameScene(L"Chamber 02") {}
	~PortalScene() override;
	PortalScene(const PortalScene& other) = delete;
	PortalScene(PortalScene&& other) noexcept = delete;
	PortalScene& operator=(const PortalScene& other) = delete;
	PortalScene& operator=(PortalScene&& other) noexcept = delete;

	enum Portal
	{
		Blue,
		Orange,
		PortalsCount,
		None
	};
	static void FirePortalInUpdate(Portal type) { m_FirePortalInUpdate = type; };

protected:
	void Initialize() override;
	void OnGUI() override;
	void Update() override;
	void Draw() override;
	virtual void OnSceneActivated()override;
	virtual void OnSceneDeactivated()override;

private:
	void MovePortal(Portal portal);
	void PlayRandomBackgroundNoise();
	friend PortalComponent;

	void CreatePortals(CameraComponent* playerCamera);
	void UpdateMenu();
	Character* m_pCharacter{};
	GameObject* m_pPlayerAnimObject{};
	std::array<GameObject*, 2> m_pPortals{};

	GameObject* m_pCube{};
	DoorComponent* m_pDoor{};

	GameObject* m_pMap;

	CrosshairComponent* m_pCrosshairComponent{nullptr};

	PortalMainMenu* m_pMenuScene{};
	GameObject* m_pMenuObject{};
	TextureData* m_pMenuTexture{};
	PostChromatic* m_pPostChromatic{};

	inline static Portal m_FirePortalInUpdate{Portal::None};

	enum class SoundLibrary 
	{
		FireOrangePortal,
		FireBluePortal,
		PortalClose1,
		PortalClose2,
		PortalOpen1,
		PortalOpen2,
		PortalOpen3,
		AmbientMain,
		AmbientRandom1,
		AmbientRandom2,
		AmbientRandom3,
		AmbientRandom4,
		ElevatorChime
	};

	std::unordered_map<SoundLibrary, FMOD::Sound*> m_pSounds{};
};

