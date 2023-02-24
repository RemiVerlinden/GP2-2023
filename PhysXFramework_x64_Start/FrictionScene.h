

#pragma once
#include "GameScene.h"

class FrictionScene final : public GameScene
{
public:
	FrictionScene() :GameScene(L"FrictionScene") {}
	~FrictionScene() override = default;
	FrictionScene(const FrictionScene& other) = delete;
	FrictionScene(FrictionScene&& other) noexcept = delete;
	FrictionScene& operator=(const FrictionScene& other) = delete;
	FrictionScene& operator=(FrictionScene&& other) noexcept = delete;

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

	void CreatePxStaticCube(GameObject*& cube, PxRigidStatic*& cubeActor,
		XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material
	);

	void CreatePxDynamicCube(GameObject*& cube, PxRigidDynamic*& cubeActor,
		XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material
	);
	std::vector<GameObject*> m_pDynamicCubeVec;
	std::vector<GameObject*> m_pStaticCubeVec;
	std::vector <PxRigidDynamic*> m_pCubeDynamicActorVec;
	std::vector <PxRigidStatic*> m_pCubeStaticActorVec;

	const int m_NrOfFrictionCubes = 3;

	XMFLOAT3 m_CubeDefaultTranslate{ -8.f, 10.f, 0.f };
	XMFLOAT3 m_CubeDefaultRotation{ 0,0,-30 };
	float m_CubeSpawnOffset = 8.f;
	float m_PlatformVerticalSpawnOffset = 8.f;
};

