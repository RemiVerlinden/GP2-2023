#include "stdafx.h"
#include "ArrowObject.h"

// Constructor: Assigns the given material and length to the ArrowObject's data members
ArrowObject::ArrowObject(BaseMaterial* pMaterial)
	: m_pMaterial{ pMaterial }
{
}

// Initialize: Sets up the ArrowObject's visual representation and its properties
void ArrowObject::Initialize(const SceneContext&)
{
	// Step 1: Create a new GameObject (pEmpty)
	GameObject* pEmpty = new GameObject();
	// Step 2: Make pEmpty a child of the ArrowObject
	AddChild(pEmpty);

	// Step 3: Add a ModelComponent to pEmpty using the Bone.ovm mesh
	ModelComponent* pModel = pEmpty->AddComponent(new ModelComponent(L"Meshes/Arrow.ovm"));
	// Step 4: Apply the ArrowObject's material to pModel
	pModel->SetMaterial(m_pMaterial);

}