#include "stdafx.h"
#include "PortalProps.h"

#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/ColorMaterial.h"
#include "Materials/Portal/PhongMaterial.h"
#include "Materials/Portal/PhongMaterial_Skinned.h"


void PortalProps::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawPhysXDebug = false;

	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.2f, 0.2f, 0.27f);
	GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	[[maybe_unused]]float scale = 0.15f;

	//{
	//	auto m_pPhong = MaterialManager::Get()->CreateMaterial<PhongMaterial>();
	//	m_pPhong->SetDiffuseTexture(L"Textures/Maps/chamber02_static/materials_models_props_button.dds");
	//	m_pPhong->SetNormalTexture(L"Textures/Maps/chamber02_static/materials_models_props_button.dds");

	//	const auto pButtonTop = AddChild(new GameObject);
	//	pModelTop = pButtonTop->AddComponent(new ModelComponent(L"Meshes/DynamicProps/button_top.ovm"));
	//	pModelTop->SetMaterial(m_pPhong);

	//	m_pButtonAnim = pButtonTop->AddComponent(new ButtonAnimComponent(pButtonTop));

	//	pButtonTop->GetTransform()->Scale(scale);

	//	const auto pButtonBase = AddChild(new GameObject);
	//	const auto pModelbase = pButtonBase->AddComponent(new ModelComponent(L"Meshes/DynamicProps/button_base.ovm"));
	//	pModelbase->SetMaterial(m_pPhong);
	//	pButtonBase->GetTransform()->Scale(scale);
	//}


	//{
	//	auto m_pPhong = MaterialManager::Get()->CreateMaterial<PhongMaterial>();
	//	m_pPhong->SetDiffuseTexture(L"Textures/Maps/chamber02_static/materials_models_props_metal_box.dds");
	//	m_pPhong->SetNormalTexture(L"Textures/Maps/chamber02_static/materials_models_props_metal_box_normal.dds");

	//	const auto pCube = AddChild(new GameObject);
	//	pModelTop = pCube->AddComponent(new ModelComponent(L"Meshes/DynamicProps/cube.ovm"));
	//	pModelTop->SetMaterial(m_pPhong);



	//	// Attach a rigidbody and collider components to the chair object
	//	PxConvexMesh* pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/DynamicProps/cube.ovpc");
	//	RigidBodyComponent* pRigidBody = pCube->AddComponent(new RigidBodyComponent());
	//	pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{PxReal{scale}} }, *pDefaultMaterial, false);

	//	PxRigidBodyExt::updateMassAndInertia(*pRigidBody->GetPxRigidActor()->is<PxRigidBody>(), 10.f);


	//	// Change the Pivot of the chair object

	//	pCube->GetTransform()->Scale(scale);

	//	m_pCube = pCube;
	//	//pCube->GetTransform()->Translate(1,1,1);
	//	m_pCube->GetTransform()->Translate(0, 15, 0);
	//	m_pCube->GetTransform()->Rotate(45, 45, 0);
	//}

	{
		auto m_pPhong = MaterialManager::Get()->CreateMaterial<PhongMaterial_Skinned>();
		m_pPhong->SetDiffuseTexture(L"Textures/Maps/chamber02_static/materials_models_props_door_01.dds");
		m_pPhong->SetNormalTexture(L"Textures/Maps/chamber02_static/materials_models_props_door_01_normal.dds");

		const auto pDoor = AddChild(new GameObject);
		pModelTop = pDoor->AddComponent(new ModelComponent(L"Meshes/DynamicProps/door.ovm"));
		pModelTop->SetMaterial(m_pPhong);



		// Attach a rigidbody and collider components to the chair object
		PxConvexMesh* pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Meshes/DynamicProps/door.ovpc");
		RigidBodyComponent* pRigidBody = pDoor->AddComponent(new RigidBodyComponent(true));
		pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh, PxMeshScale{PxReal{scale}} }, *pDefaultMaterial, false);

		pDoor->GetTransform()->Scale(scale);

	}
}

void PortalProps::OnGUI()
{
}

void PortalProps::Update()
{
	//if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'N'))
	//{
	//	static bool pressed = false;
	//	pressed = !pressed;
	//	m_pButtonAnim->SetPressed(pressed);
	//}


	if (m_SceneContext.pInput->IsKeyboardKey(InputState::pressed, 'K'))
	{
		const auto pCameraTransform = m_SceneContext.pCamera->GetTransform();
		m_SceneContext.pLights->SetDirectionalLight(pCameraTransform->GetPosition(), pCameraTransform->GetForward());
	}
}