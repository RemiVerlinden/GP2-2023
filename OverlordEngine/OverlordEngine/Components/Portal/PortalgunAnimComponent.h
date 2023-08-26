#pragma once
class PhongMaterial_Skinned;
class GameObject;
class Character;
class ViewBob;
class PortalGun_Glow_Material;
class PortalgunAnimComponent final : public BaseComponent
{
public:
	PortalgunAnimComponent(Character* character);
	~PortalgunAnimComponent() override = default;

	enum AnimationState
	{
		Draw,
		Fire,
		Fizzle,
		Holster,
		Idle,
		IdleToLow,
		LowIdle,
		LowToIdle,
		Pickup,
		Release
	};

	void SetAnimation(AnimationState state, bool playOnce = false);
	bool IsAnimationFinished() const;
	void SetPortalgunColor(const XMFLOAT3& color);
	void TogglePortalgunModelRender(bool shouldRender); // when you are using a different camera than player camera, the portalgun should be invisible
protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;

private:

	void CreatePortalgunMesh();

	std::unique_ptr<ViewBob> m_pViewBob{nullptr};

	GameObject* m_pPortalgunHolder = nullptr;
	Character* m_pCharacter = nullptr;
	PhongMaterial_Skinned* m_pFrame = nullptr;
	ModelAnimator* m_pAnimator = nullptr;

	PortalGun_Glow_Material* m_pPortalgunGlowMaterial{};
	float m_PortalColor[3] = { 0.f, 0.6f, 1.f };
};
