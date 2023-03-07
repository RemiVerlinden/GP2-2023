#pragma once
#include "GameScene.h"
#include <vector>

class BouncingBallScene final : public GameScene
{
public:
	BouncingBallScene() :GameScene(L"BouncingBallScene") {}
	~BouncingBallScene() override = default;
	BouncingBallScene(const BouncingBallScene& other) = delete;
	BouncingBallScene(BouncingBallScene&& other) noexcept = delete;
	BouncingBallScene& operator=(const BouncingBallScene& other) = delete;
	BouncingBallScene& operator=(BouncingBallScene&& other) noexcept = delete;

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
	std::vector<GameObject*> m_pSpheres;
	std::vector < PxRigidDynamic*> m_pSphereActors;

	const int m_NrOfSpheres = 3;

	XMFLOAT3 m_SphereSpawnLocation{ -5,8,0 };
	float m_SphereSpawnOffset = 5;
};

