#pragma once
struct CharacterDesc
{
	CharacterDesc(
		PxMaterial* pMaterial,
		float radius = .5f,
		float height = 2.f)
	{
		controller.setToDefault();
		controller.halfSideExtent = radius;
		controller.halfForwardExtent = radius;
		controller.halfHeight = height / 2.f + radius;
		controller.material = pMaterial;
		controller.contactOffset = .01f;
		controller.slopeLimit = .2f;
	}

	inline static float maxMoveSpeed{ 7.f };
	float maxFallSpeed{ 15.f };

	float JumpSpeed{ 15.f };

	float moveAccelerationTime{ .2f };
	float fallAccelerationTime{ .3f };

	PxBoxControllerDesc controller{};

	float rotationSpeed{ 60.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Jump{ -1 };
};

class PlayerAnimComponent;
class PortalgunAnimComponent;
class Character : public GameObject
{
public:
	Character(const CharacterDesc& characterDesc);
	~Character() override = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	const XMFLOAT3& GetVelocity() const { return m_TotalVelocity; }
	float GetPitch() const { return m_TotalPitch; }
	float GetYaw() const { return m_TotalYaw; }
	float GetCharacterHalfHeight() const { return m_CharacterDesc.controller.halfHeight; }
	void DrawImGui();
	CameraComponent* GetCameraComponent() const { return m_pCameraComponent; }
	void InitializeCharacterMeshes(); // this will create the player mesh and portalgun mesh and their animations

	static const float GetMaxPickupDistance() { return m_MaxPickupDistance; }
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
private:
	void InitCharacterSettings();
private:
	void UpdatePlayerAnimationState(const SceneContext& sceneContext,  bool isGrounded);
	void UpdatePortalgunAnimationState(const SceneContext& sceneContext);

	void EnableNoclipCamera(bool enable);
	GameObject* m_pCameraHolder{};
	CameraComponent* m_pCameraComponent{};
	ControllerComponent* m_pControllerComponent{};

	GameObject* m_pPlayerAnimObject{};
	PlayerAnimComponent* m_pPlayerAnimComponent{};
	PortalgunAnimComponent* m_pPortalgunAnimComponent{};

	CharacterDesc m_CharacterDesc;
	float m_TotalPitch{}, m_TotalYaw{};				//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)
	float m_MouseSensitivity{};						//Mouse Sensitivity for rotation
	bool m_IsCharacterMeshesInitialized{ false };

	const static inline float m_MaxPickupDistance = 2.5f;
};

