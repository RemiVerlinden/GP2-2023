#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

void SoftwareSkinningScene_3::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;

	ColorMaterial* pColorMat = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	GameObject* pRoot = new GameObject();
	AddChild(pRoot);
	constexpr float boneLength{ 15.f };
	m_pBone0 = new BoneObject(pColorMat, boneLength);
	pRoot->AddChild(m_pBone0);
	m_pBone1 = new BoneObject(pColorMat, boneLength);
	m_pBone0->AddBone(m_pBone1);

	GameObject* pBoxDrawer = new GameObject();
	AddChild(pBoxDrawer);
	m_pMeshDrawer = pBoxDrawer->AddComponent(new MeshDrawComponent(24, true));
	m_pBone0->CalculateBindPose();
	InitializeVertices(boneLength);
}

void SoftwareSkinningScene_3::Update()
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

	// Update BoneTransforms
	const XMMATRIX bone0Transform{ XMLoadFloat4x4(&m_pBone0->GetBindPose()) * XMLoadFloat4x4(&m_pBone0->GetTransform()->GetWorld()) };
	const XMMATRIX bone1Transform{ XMLoadFloat4x4(&m_pBone1->GetBindPose()) * XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld()) };

	for (int vertexNr{}; vertexNr < m_SkinnedVertices.size(); ++vertexNr)
	{
		VertexSoftwareSkinned& skinnedVertex = m_SkinnedVertices[vertexNr];

		if (skinnedVertex.blendWeight >= 0.0f && skinnedVertex.blendWeight <= 1.0f)
		{
			const XMVECTOR transformedVertexBone0{ XMVector3TransformCoord(XMLoadFloat3(&skinnedVertex.originalVertex.Position), bone0Transform) };
			const XMVECTOR transformedVertexBone1{ XMVector3TransformCoord(XMLoadFloat3(&skinnedVertex.originalVertex.Position), bone1Transform) };
			XMStoreFloat3(&skinnedVertex.transformedVertex.Position, XMVectorLerp(transformedVertexBone1, transformedVertexBone0, skinnedVertex.blendWeight));
		}
	}

	// Update visuals

	m_pMeshDrawer->RemoveTriangles();

	for (size_t vertexNr{ 0 }; vertexNr < m_SkinnedVertices.size(); vertexNr += 4)
	{
		QuadPosNormCol quad{ m_SkinnedVertices[vertexNr].transformedVertex,m_SkinnedVertices[vertexNr + 1].transformedVertex,m_SkinnedVertices[vertexNr + 2].transformedVertex,m_SkinnedVertices[vertexNr + 3].transformedVertex };
		m_pMeshDrawer->AddQuad(quad);
	}

	m_pMeshDrawer->UpdateBuffer();
}

XMFLOAT3 SoftwareSkinningScene_3::QuaternionToYawPitchRoll(const XMFLOAT4& quaternion)
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

void SoftwareSkinningScene_3::OnGUI()
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

void SoftwareSkinningScene_3::InitializeVertices(float length)
{
	auto pos = XMFLOAT3(length / 2.f, 0.f, 0.f);
	const auto offset = XMFLOAT3(length / 2.f, 2.5f, 2.5f);
	auto col = XMFLOAT4(1.f, 0.f, 0.f, 0.5f);
#pragma region BOX 1
	//FRONT
	XMFLOAT3 norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
#pragma endregion

	col = { 0.f, 1.f, 0.f, 0.5f };
	pos = { 22.5f, 0.f, 0.f };
#pragma region BOX 2
	//FRONT
	norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.0f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.0f);
#pragma endregion
}
