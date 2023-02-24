#pragma once
#include "GameScene.h"
class TorqueScene final : public GameScene
{
public:
	TorqueScene() :GameScene(L"TorqueScene") {}
	~TorqueScene() override = default;
	TorqueScene(const TorqueScene& other) = delete;
	TorqueScene(TorqueScene&& other) noexcept = delete;
	TorqueScene& operator=(const TorqueScene& other) = delete;
	TorqueScene& operator=(TorqueScene&& other) noexcept = delete;

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
	GameObject* m_pDefaultCube{ nullptr };

	PxRigidDynamic* m_pCubeActor;
	float m_TorqueForce{ 2000.f };


	XMFLOAT3 m_CubeDimentions{ 4,4,4 };
	XMFLOAT3 m_MaterialValues{ 0.5f,0.5f,0.f };

};

