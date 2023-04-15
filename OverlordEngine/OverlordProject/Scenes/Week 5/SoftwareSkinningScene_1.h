#pragma once

class BoneObject;

class SoftwareSkinningScene_1 : public GameScene
{
public:
	SoftwareSkinningScene_1() :GameScene(L"SoftwareSkinningScene_1") {}
	~SoftwareSkinningScene_1() override = default;
	SoftwareSkinningScene_1(const SoftwareSkinningScene_1& other) = delete;
	SoftwareSkinningScene_1(SoftwareSkinningScene_1&& other) noexcept = delete;
	SoftwareSkinningScene_1& operator=(const SoftwareSkinningScene_1& other) = delete;
	SoftwareSkinningScene_1& operator=(SoftwareSkinningScene_1&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	struct RotationSettings // all the bone animation settings
	{
		inline static const float rotationSpeed = 45;
		inline static const float autoLowerBound = -45;
		inline static const float autoUpperBound = 45;
		inline static const float manualLowerBound = -90;
		inline static const float manualUpperBound = 90;
	};  
	XMFLOAT3 QuaternionToYawPitchRoll(const XMFLOAT4& quaternion);

	BoneObject* m_pBone0{};
	BoneObject* m_pBone1{};


	bool m_GuiManualMode{};
};


