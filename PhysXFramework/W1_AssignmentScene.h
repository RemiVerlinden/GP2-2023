#pragma once
#include "GameScene.h"
class W1_AssignmentScene final : public GameScene
{
public:
	W1_AssignmentScene() :GameScene(L"W1_AssignmentScene") {}
	~W1_AssignmentScene() override = default;
	W1_AssignmentScene(const W1_AssignmentScene& other) = delete;
	W1_AssignmentScene(W1_AssignmentScene&& other) noexcept = delete;
	W1_AssignmentScene& operator=(const W1_AssignmentScene& other) = delete;
	W1_AssignmentScene& operator=(W1_AssignmentScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:

	void CreatePxDynamicCube(GameObject*& cube, PxRigidDynamic*& cubeActor,
							XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material
							);
	void CreatePxStaticCube(GameObject*& cube, PxRigidStatic*& cubeActor,
		XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material
	);
	void CreatePxStaticCube(GameObject*& cube, PxRigidStatic*& cubeActor,
		XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material, XMFLOAT4 color
	);
	void CreatePxDynamicSphere(GameObject*& sphere, PxRigidDynamic*& sphereActor,
		XMFLOAT3 pos, XMFLOAT3 rotation, float radius, XMFLOAT3 material
	);


	enum InputIds
	{
		reset,
		moveLeft,
		moveRight,
		moveUp,
		moveDown,
		jump
	};

	std::vector<GameObject*> m_pCubeVec;
	std::vector<PxRigidDynamic*> m_pCubeActorVec;

	GameObject* m_pSphere;
	PxRigidDynamic* m_pSphereActor;

	XMFLOAT3 m_CubeDimentions{ 1,1,1 };

	int m_TotalHorizontalCubes = 10;
	int m_TotalVerticalCubes = 10;

	float m_CubeWallSpawnDistance{ 30.f };
	float m_CubeWiggleRoom = 0.12f; // horizontal distance between each cube

	float m_SphereRadius{1.f};
	int m_MaxRandomSpawnRotation{ 20 }; // in degrees
	float m_SphereTorqueForce = 10000.f;
	float m_SphereJumpForce = 14000.f;
	float m_SphereMass = 26.f;

	XMFLOAT3 m_CubeMatValues{ 0.5f,0.5f,0.f };
	XMFLOAT3 m_SphereMatValues{ 0.7f,0.7f,0.f };
	XMFLOAT3 m_GroundMatValues{ 0.5f,0.5f,0.f };

	XMFLOAT3 m_FloorCubeSize{50,5,70};
	XMFLOAT4 m_FloorColor{Colors::DarkGray};


};

