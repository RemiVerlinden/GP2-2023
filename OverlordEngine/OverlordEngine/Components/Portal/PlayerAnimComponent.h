#pragma once
class PhongMaterial_Skinned;
class GameObject;
class Character;
class PlayerAnimComponent final : public BaseComponent
{
public:
	PlayerAnimComponent(Character* character);
	~PlayerAnimComponent() override = default;

	enum AnimationState
	{
		NoGun_AirWalk,
		NoGun_JumpFloat,
		NoGun_RunE,
		NoGun_RunN,
		NoGun_RunNE,
		NoGun_RunNW,
		NoGun_RunS,
		NoGun_RunSE,
		NoGun_RunSW,
		NoGun_RunW,
		NoGun_StandingIdle,
		NoGun_StandingJump
	};

	void SetAnimation(AnimationState state);
protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;

private:
	void CreatePlayerMesh();

	Character* m_pCharacter = nullptr;
	PhongMaterial_Skinned* m_pFrame = nullptr;
	ModelAnimator* m_pAnimator = nullptr;
	AnimationState m_AnimationState = AnimationState::NoGun_StandingIdle;
};
