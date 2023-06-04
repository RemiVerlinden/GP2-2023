#include "stdafx.h"
#include "DoorComponent.h"
#include "../OverlordProject/Materials/Portal/PhongMaterial_Skinned.h"
void DoorComponent::Initialize(const SceneContext&)
{
	static MapLoader::DoorProperties props;

	m_pDoormaterial = MaterialManager::Get()->CreateMaterial<PhongMaterial_Skinned>();
	m_pDoormaterial->SetDiffuseTexture(props.diffuseMapPath);
	m_pDoormaterial->SetNormalTexture(props.normalMapPath);

	m_pDoorSides = std::make_pair(CreateDoorSide(false), CreateDoorSide(true));
}

void DoorComponent::SetDoorState(bool open)
{
	if (!m_IsInitialized) return;
	if (m_IsOpen == open) return;
	m_IsOpen = open;
	
	UINT clip = open ? 1 : 0;
	for (GameObject* pDoor : { m_pDoorSides.first ,m_pDoorSides.second})
	{
		ModelAnimator* pDoorAnimator = pDoor->GetComponent<ModelComponent>()->GetAnimator();
		pDoorAnimator->SetAnimation(clip);
		pDoorAnimator->PlayOnce();
	}
}
// not my fault, collisionIgnore groups do NOT work it doesnt work I tried getting it to work it doesnt
// SetCollisionGroup(static_cast<CollisionGroup>(m_CollisionGroups.word0));
// SetCollisionIgnoreGroup(static_cast<CollisionGroup>(m_CollisionGroups.word1));
void DoorComponent::SetDoorCollision(bool enable)
{
	// I DID NOT WANT TO DO IT LIKE THIS BUT YOU GIVE ME NO OTHER OPTION
	const static XMFLOAT3 disabledCollisionPos{ D3D11_FLOAT32_MAX/2, D3D11_FLOAT32_MAX/2, D3D11_FLOAT32_MAX/2 };

	const static XMFLOAT3 originalCollisionPos = m_pGameObject->GetTransform()->GetPosition();

	for (GameObject* pDoor : { m_pDoorSides.first ,m_pDoorSides.second })
	{
		if (enable)
			pDoor->GetComponent<RigidBodyComponent>()->Translate(originalCollisionPos);
		else
			pDoor->GetComponent<RigidBodyComponent>()->Translate(disabledCollisionPos);
		

	}
}

void DoorComponent::StartInteraction() 
{
	SetDoorState(true);
}

void DoorComponent::EndInteraction()
{
	SetDoorState(false);
}

void DoorComponent::Update(const SceneContext& /*context*/)
{
	static bool wasOpen = m_IsOpen;

	if (wasOpen != m_IsOpen)
	{
		wasOpen = m_IsOpen;
		SetDoorCollision(!m_IsOpen);
	}
}

GameObject* DoorComponent::CreateDoorSide(bool front)
{
	float scale = 0.041f; // I was unable to scale my object in 3d program without breaking the animation for OVM converter so this will have to do for now

	static MapLoader::DoorProperties props;

	static PxMaterial* pMaterial = PxGetPhysics().createMaterial(props.pxMaterial[0], props.pxMaterial[1], props.pxMaterial[2]);

	GameScene& scene = *GetGameObject()->GetScene();
	GameObject* pDoor = scene.AddChild(new GameObject());
	ModelComponent* pModel = pDoor->AddComponent(new ModelComponent(props.modelPath));
	pModel->SetMaterial(m_pDoormaterial);

	ModelAnimator* pAnimator = pModel->GetAnimator();
	pAnimator->SetAnimation(0);
	pAnimator->PlayOnce();


	PxConvexMesh* pConvexMesh = ContentManager::Load<PxConvexMesh>(props.rigidBodyPath);
	RigidBodyComponent* pRigidBody = pDoor->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{scale} }, *pMaterial, false);


	pDoor->GetTransform()->Translate(m_pGameObject->GetTransform()->GetPosition());
	pDoor->GetTransform()->Scale(scale, scale, scale);

	if (front) pDoor->GetTransform()->Rotate(0.f, 180.f, 0.f); // should be clear

	return pDoor;
}




