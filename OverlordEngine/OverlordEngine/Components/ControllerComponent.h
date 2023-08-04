#pragma once

class ControllerComponent : public BaseComponent
{
public:
	ControllerComponent(const PxBoxControllerDesc& controllerDesc);
	~ControllerComponent() override = default;

	ControllerComponent(const ControllerComponent& other) = delete;
	ControllerComponent(ControllerComponent&& other) noexcept = delete;
	ControllerComponent& operator=(const ControllerComponent& other) = delete;
	ControllerComponent& operator=(ControllerComponent&& other) noexcept = delete;

	void Translate(const XMFLOAT3& pos) const;
	void Move(const XMFLOAT3& displacement, float minDistance = 0);

	void SetCollisionGroup(CollisionGroup groups);
	void SetCollisionIgnoreGroup(CollisionGroup ignoreGroups);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetFootPosition() const;
	PxControllerCollisionFlags GetCollisionFlags() const {return m_CollisionFlag;}
	PxController* GetPxController() const { return m_pController; }

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void OnSceneDetach(GameScene*) override;
	void OnSceneAttach(GameScene*) override;
	void OnOwnerAttach(GameObject*) override;
	void OnOwnerDetach(GameObject*) override;

private:

	PxBoxControllerDesc m_ControllerDesc{};

	PxController* m_pController{};
	PxControllerCollisionFlags m_CollisionFlag{};
	PxFilterData m_CollisionGroups{};

	void ApplyFilterData() const;
};

