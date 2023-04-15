#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"
#include <cmath>

void SoftwareSkinningScene_1::Initialize()
{
    m_SceneContext.settings.enableOnGUI = true;

    // Create a shared ColorMaterial for BoneObjects
    ColorMaterial* pColorMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();

    // Create root GameObject for the bone hierarchy
    GameObject* pRoot = new GameObject();
    AddChild(pRoot);

    // Initialize the first bone with shared ColorMaterial and a length of 15.f
    const float boneLength{ 15.f };
    m_pBone0 = new BoneObject(pColorMat, boneLength);
    pRoot->AddChild(m_pBone0);

    // Initialize the second bone with the same parameters as the first bone
    m_pBone1 = new BoneObject(pColorMat, boneLength);
    m_pBone0->AddBone(m_pBone1);

}

void SoftwareSkinningScene_1::Update()
{
    static float boneRotation{};
    static int rotationSign{ 1 };
    static XMFLOAT3 bone0Rotation{}, bone1Rotation{};

    if (!m_GuiManualMode)
    {
        // Update bone rotation

        boneRotation += RotationSettings::rotationSpeed * m_SceneContext.pGameTime->GetElapsed() * rotationSign;
        if ((boneRotation > RotationSettings::autoUpperBound) || (boneRotation < RotationSettings::autoLowerBound))
        {
            boneRotation = RotationSettings::autoUpperBound * rotationSign;
            rotationSign = -rotationSign;
        }
        bone0Rotation.z = boneRotation;
        bone1Rotation.z = -boneRotation * 2.f;

        // Rotate both bones along the Z-axis
        m_pBone0->GetTransform()->Rotate(bone0Rotation);
        m_pBone1->GetTransform()->Rotate(bone1Rotation);
    }
}

XMFLOAT3 SoftwareSkinningScene_1::QuaternionToYawPitchRoll(const XMFLOAT4& quaternion)
{
    XMFLOAT3 retVector;

    const auto x = quaternion.x;
    const auto y = quaternion.y;
    const auto z = quaternion.z;
    const auto w = quaternion.w;

    retVector.x = atan2(2.0f * (y * z + w * x), w * w - x * x - y * y + z * z);
    retVector.y = asin(-2.0f * (x * z - w * y));
    retVector.z = atan2(2.0f * (x * y + w * z), w * w + x * x - y * y - z * z);

    retVector.x = retVector.x * (180.0f / XM_PI);
    retVector.y = retVector.y * (180.0f / XM_PI);
    retVector.z = retVector.z * (180.0f / XM_PI);

    return retVector;
}

void SoftwareSkinningScene_1::OnGUI()
{
    static XMFLOAT3 bone0Rotation{}, bone1Rotation{};

    // Checkbox for toggling between manual and automatic mode
    if (ImGui::Checkbox("Manual mode", &m_GuiManualMode))
    {
        bone0Rotation = QuaternionToYawPitchRoll(m_pBone0->GetTransform()->GetRotation());
        bone1Rotation = QuaternionToYawPitchRoll(m_pBone1->GetTransform()->GetRotation());
    }

    // Manual mode sliders for adjusting bone rotations
    if (m_GuiManualMode)
    {
        ImGui::SliderFloat3("Bone0 Rot", ConvertUtil::ToImFloatPtr(bone0Rotation), RotationSettings::manualLowerBound, RotationSettings::manualUpperBound);
        ImGui::SliderFloat3("Bone1 Rot", ConvertUtil::ToImFloatPtr(bone1Rotation), RotationSettings::manualLowerBound, RotationSettings::manualUpperBound);

        // Apply manual rotations to bones
        m_pBone0->GetTransform()->Rotate(bone0Rotation);
        m_pBone1->GetTransform()->Rotate(bone1Rotation);
    }
}
