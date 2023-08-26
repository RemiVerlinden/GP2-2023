#pragma once
class PhongMaterial_Skinned;
class GameObject;
class PickUpObjectComponent final : public BaseComponent
{
public:
	PickUpObjectComponent() = default;
	~PickUpObjectComponent() override = default;

	void SetGrabbed(bool grabbed);
	bool IsGrabbed() const { return m_IsGrabbed; }
protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;
	void OnContact(float impactForce) override;
private:
	bool m_IsGrabbed = false;
	float m_ImpactThreshholdSoft{};
	float m_ImpactThreshholdHard{};


	RigidBodyComponent* m_pRigidBody = nullptr;


	enum class SoundLibrary
	{
		Grab,
		impactSoft1,
		impactSoft2,
		impactSoft3,
		impactHard1,
		impactHard2,
		impactHard3
	};

	static inline std::unordered_map<SoundLibrary, FMOD::Sound*> m_pSounds{};


	enum class impactType
	{
		soft,
		hard
	};
	SoundLibrary GetRandomSound(impactType impactType);
};
