#pragma once
#include "GameScene.h"
class W2_AssignmentScene final : public GameScene
{
public:
	W2_AssignmentScene() :GameScene(L"W2_AssignmentScene") {}
	~W2_AssignmentScene() override = default;
	W2_AssignmentScene(const W2_AssignmentScene& other) = delete;
	W2_AssignmentScene(W2_AssignmentScene&& other) noexcept = delete;
	W2_AssignmentScene& operator=(const W2_AssignmentScene& other) = delete;
	W2_AssignmentScene& operator=(W2_AssignmentScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

	void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
private:
	// these functions should be global or at least much more accesible
	// I just kinda dont know how to do that properly and dont want to find out so this is how we do things for now
	void CreatePxDynamicCube(GameObject*& cube, PxRigidDynamic*& cubeActor,
		XMFLOAT3 pos, XMFLOAT3 rotation, XMFLOAT3 size, XMFLOAT3 material, XMFLOAT4 color
	);
	void CreatePxDynamicSphere(GameObject*& sphere, PxRigidDynamic*& sphereActor,
		XMFLOAT3 pos, XMFLOAT3 rotation, float radius, XMFLOAT3 material, XMFLOAT4 color
	);

	void LockAxisZ(PxRigidDynamic* actor);

private:
	GameObject* m_pConvexChair{ nullptr };
	std::vector<GameObject*> m_pDynamicCubeVec;
	std::vector<GameObject*> m_pDynamicSphereVec;
	std::vector<GameObject*> m_pTrapdoorVec;
	XMFLOAT3 m_TrapDoorSphereSpawnPos{ -9.5f,20,0 };
	XMFLOAT3 m_TrapDoorSpawnPos1{ -8.85f, 17.05f, 0 };
	XMFLOAT3 m_TrapDoorSpawnPos2{ 8.9f, 17.05f, 0 };
	XMFLOAT3 m_DynamicCubeSpawnPos{ -4,5,0 };

	GameObject* m_pTriangleLevel{ nullptr };
	PxRigidDynamic* m_pTriangleActor{ nullptr };
	PxVec3 m_KinematicPosition{};

	GameObject* m_pSphere;
	XMFLOAT3 m_SpherePos{ 0, 15, 0 };
	float m_SphereForce = 5000.f;
	float m_SphereMass = 10.f;
	float m_SphereJumpForce{5000.f};

	PxRigidStatic* m_pTriggerLeft{ nullptr };
	PxRigidStatic* m_pTriggerRight{ nullptr };

	GameObject* temp;

	PxRevoluteJoint* m_pLeftfJoint;
	PxRevoluteJoint* m_pRightJoint;
	bool m_EnableLeftJoint = false;
	bool m_EnableRightJoint = false;

	FMOD::Channel* m_pChannel2D{};
	
	FMOD::Sound* m_pLeftBell;
	FMOD::Sound* m_pRightBell;
};



