#pragma once
#include "GameScene.h"

class PhysXTestScene final : public GameScene
{
public:
	PhysXTestScene() :GameScene(L"PhysXTestScene") {}
	~PhysXTestScene() override = default;
	PhysXTestScene(const PhysXTestScene & other) = delete;
	PhysXTestScene(PhysXTestScene && other) noexcept = delete;
	PhysXTestScene& operator=(const PhysXTestScene & other) = delete;
	PhysXTestScene& operator=(PhysXTestScene && other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	enum InputIds
	{
		reset
	};

	GameObject* m_pCube{nullptr};
	PxRigidDynamic* m_pCubeActor{ nullptr };

	XMFLOAT3 m_CubeDefaultTranslate{ 0.f, 5.f, 0.f };
	XMFLOAT3 m_CubeDefaultRotation{ 45,30,30 };
};

