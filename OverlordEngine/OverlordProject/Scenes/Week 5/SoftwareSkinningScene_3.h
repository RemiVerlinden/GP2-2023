#pragma once
class BoneObject;

class SoftwareSkinningScene_3 : public GameScene
{
public:
	SoftwareSkinningScene_3() :GameScene(L"SoftwareSkinningScene_3") {}
	~SoftwareSkinningScene_3() override = default;

	SoftwareSkinningScene_3(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3(SoftwareSkinningScene_3&& other) noexcept = delete;
	SoftwareSkinningScene_3& operator=(const SoftwareSkinningScene_3& other) = delete;
	SoftwareSkinningScene_3& operator=(SoftwareSkinningScene_3&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	XMFLOAT3 QuaternionToYawPitchRoll(const XMFLOAT4& quaternion);

	struct RotationSettings // all the bone animation settings
	{
		inline static const float rotationSpeed = 45;
		inline static const float autoLowerBound = -45;
		inline static const float autoUpperBound = 45;
		inline static const float manualLowerBound = -90;
		inline static const float manualUpperBound = 90;
	};

	BoneObject* m_pBone0{};
	BoneObject* m_pBone1{};

	bool m_GuiManualMode{};

	struct VertexSoftwareSkinned
	{
		VertexSoftwareSkinned(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT4 color, float blendweight)
			:transformedVertex{ position,normal,color }
			, originalVertex{ position,normal,color }
			,blendWeight{blendweight}
		{
		}

		VertexPosNormCol transformedVertex{};
		VertexPosNormCol originalVertex{};
		float blendWeight{};
	};

	void InitializeVertices(float lenght);

	MeshDrawComponent* m_pMeshDrawer{};
	std::vector<VertexSoftwareSkinned> m_SkinnedVertices{ };
};

