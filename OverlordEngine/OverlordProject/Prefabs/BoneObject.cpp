#include "stdafx.h"
#include "BoneObject.h"

// Constructor: Assigns the given material and length to the BoneObject's data members
BoneObject::BoneObject(BaseMaterial* pMaterial, float length)
	: m_Length{ length }
	, m_pMaterial{ pMaterial }
{
}

// AddBone: Connects the given bone to this BoneObject and positions it correctly
void BoneObject::AddBone(BoneObject* pBone)
{
	// Move the new bone along the X-axis based on the parent bone's length
	pBone->GetTransform()->Translate(m_Length, 0, 0);
	// Establish a parent-child relationship between the bones
	AddChild(pBone);
}

// CalculateBindPose: Calculates the bind pose of the BoneObject and all its children
void BoneObject::CalculateBindPose()
{
	// Calculate the inverse world matrix of the current BoneObject
	const XMMATRIX inverseWorldMatrix{ XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetTransform()->GetWorld())) };

	// Store the inverse world matrix as the bind pose
	XMStoreFloat4x4(&m_BindPose, inverseWorldMatrix);

	// Recursively calculate and store the bind pose for all child BoneObjects
	for (BoneObject* pChild : GetChildren<BoneObject>())
	{
		pChild->CalculateBindPose();
	}
}

// Initialize: Sets up the BoneObject's visual representation and its properties
void BoneObject::Initialize(const SceneContext&)
{
	// Step 1: Create a new GameObject (pEmpty)
	GameObject* pEmpty = new GameObject();
	// Step 2: Make pEmpty a child of the BoneObject
	AddChild(pEmpty);

	// Step 3: Add a ModelComponent to pEmpty using the Bone.ovm mesh
	ModelComponent* pModel = pEmpty->AddComponent(new ModelComponent(L"Meshes/Skeletal/Bone.ovm"));
	// Step 4: Apply the BoneObject's material to pModel
	pModel->SetMaterial(m_pMaterial);

	// Step 5: Rotate pEmpty -90 degrees around the Y-axis to align the bone with the X-axis
	pEmpty->GetTransform()->Rotate(0, -90, 0, true);
	// Step 6: Scale pEmpty uniformly to match the BoneObject's length
	pEmpty->GetTransform()->Scale(m_Length);
}