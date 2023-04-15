#pragma once
#include "SoftwareSkinningScene_1.h"

class SoftwareSkinningScene_2 : public GameScene
{
public:
    SoftwareSkinningScene_2() :GameScene(L"SoftwareSkinningScene_2") {}
    ~SoftwareSkinningScene_2() override = default;

    SoftwareSkinningScene_2(const SoftwareSkinningScene_2& other) = delete;
    SoftwareSkinningScene_2(SoftwareSkinningScene_2&& other) noexcept = delete;
    SoftwareSkinningScene_2& operator=(const SoftwareSkinningScene_2& other) = delete;
    SoftwareSkinningScene_2& operator=(SoftwareSkinningScene_2&& other) noexcept = delete;

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
        VertexSoftwareSkinned(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT4 color)
            :transformedVertex{ position,normal,color }
            , originalVertex{ position,normal,color }
        {
        }

        VertexPosNormCol transformedVertex{};
        VertexPosNormCol originalVertex{};
    };

    void InitializeVertices(float lenght);

    MeshDrawComponent* m_pMeshDrawer{};
    std::vector<VertexSoftwareSkinned> m_SkinnedVertices{ };
};
