#pragma once
#include "GameScene.h"
class ForcesScene final : public GameScene
{
public:
	ForcesScene() :GameScene(L"ForcesScene") {}
	~ForcesScene() override = default;
	ForcesScene(const ForcesScene& other) = delete;
	ForcesScene(ForcesScene&& other) noexcept = delete;
	ForcesScene& operator=(const ForcesScene& other) = delete;
	ForcesScene& operator=(ForcesScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:

	enum InputIds
	{
		reset,
		moveLeft,
		moveRight,
		moveUp,
		moveDown
	};

	GameObject* m_pCube{ nullptr };

	PxRigidDynamic* m_pCubeActor;

	float m_PushForce{800.f};

	XMFLOAT3 m_CubeDimentions{ 2,2,2 };
	XMFLOAT3 m_MaterialValues{ 0.2f,0.2f,0.f };

};

