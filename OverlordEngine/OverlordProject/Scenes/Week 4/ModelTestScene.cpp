//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"
#include "ModelTestScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

#include "Materials/DiffuseMaterial.h"
#include <algorithm>
#include <random>

ModelTestScene::ModelTestScene() :
	GameScene(L"ModelTestScene")
{
}

//void ModelTestScene::Initialize()
//{
//	m_SceneContext.settings.enableOnGUI = true;
//
//	auto physX = PhysXManager::Get()->GetPhysics();
//
//	auto pBouncyMaterial = physX->createMaterial(0, 0, 1);
//	auto pGround = new GameObject();
//	auto pRigidBodyGround = pGround->AddComponent(new RigidBodyComponent(true));
//
//	pRigidBodyGround->AddCollider(physx::PxPlaneGeometry{}, *pBouncyMaterial, false, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1))));
//	AddChild(pGround);
//}

void ModelTestScene::Initialize()
{
	InitializeGroundPlane();
	InitializeChairObject();
}

void ModelTestScene::InitializeGroundPlane()
{
	m_SceneContext.settings.enableOnGUI = false;

	// Create the ground plane with a bouncy material
	PxPhysics& physX = PxGetPhysics();

	PxMaterial* pFrictionMaterial = physX.createMaterial(0.2f, 0.2f, 0);
	GameSceneExt::CreatePhysXGroundPlane(*this, pFrictionMaterial);
}

void ModelTestScene::InitializeChairObject()
{
	PxPhysics& physX = PxGetPhysics();
	PxMaterial* pFrictionMaterial = physX.createMaterial(0, 0, 0.3f);

	// Create the chair object with a model component
	m_pChair = new GameObject();
	AddChild(m_pChair);

	ModelComponent* model = new ModelComponent(L"Labs/Week4/Meshes/Chair.ovm");
	m_pChair->AddComponent(model);

	ResetChairPosRot();


	// Apply a diffuse material to the chair model
	DiffuseMaterial* diffuse = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	diffuse->SetDiffuseTexture(L"Labs/Week4/Textures/Chair_Dark.dds");
	model->SetMaterial(diffuse);

	// Attach a rigidbody and collider components to the chair object
	PxConvexMesh* pConvexMesh = ContentManager::Load<PxConvexMesh>(L"Labs/Week4/Meshes/Chair.ovpc");
	RigidBodyComponent* pRigidBody = m_pChair->AddComponent(new RigidBodyComponent());
	pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh }, *pFrictionMaterial);

	// Change the Pivot of the chair object
	PxRigidBody* rigidBody = pRigidBody->GetPxRigidActor()->is<PxRigidBody>();
	PxVec3 massLocalPos{ 0.f,4.f,0.f };
	PxRigidBodyExt::updateMassAndInertia(*rigidBody, 10.f, &massLocalPos);
}




void ModelTestScene::Update()
{
	//Optional
}

void ModelTestScene::Draw()
{
	//Optional
}

void ModelTestScene::OnGUI()
{
	static ImVec4 colorA = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	static ImVec4 colorB = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	static bool cycleColors = false;
	static float cycleSpeed = 1.0f;

	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
	ImGui::Checkbox("Cycle Colors", &cycleColors);

	if (cycleColors)
	{
		ImGui::ColorPicker3("Color A", (float*)&colorA);
		ImGui::ColorPicker3("Color B", (float*)&colorB);
		ImGui::SliderFloat("Cycle Speed", &cycleSpeed, 0.0f, 5.0f);

		float t = 0.5f * (1.0f + std::sin(cycleSpeed * m_SceneContext.pGameTime->GetTotal()));

		ImVec4 hsvColorA, hsvColorB;
		ImGui::ColorConvertRGBtoHSV(colorA.x, colorA.y, colorA.z, hsvColorA.x, hsvColorA.y, hsvColorA.z);
		ImGui::ColorConvertRGBtoHSV(colorB.x, colorB.y, colorB.z, hsvColorB.x, hsvColorB.y, hsvColorB.z);

		ImVec4 lerpedHSV = LerpColors(hsvColorA, hsvColorB, t);
		ImVec4 lerpedRGB;
		ImGui::ColorConvertHSVtoRGB(lerpedHSV.x, lerpedHSV.y, lerpedHSV.z, lerpedRGB.x, lerpedRGB.y, lerpedRGB.z);

		m_SceneContext.settings.clearColor = { lerpedRGB.x, lerpedRGB.y, lerpedRGB.z, 1.0f };
	}
}

int RandomSign()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(0, 1);

	// If the random number is 0, return -1, otherwise return 1
	return dis(gen) == 0 ? -1 : 1;
}

void ModelTestScene::OnSceneActivated()
{
	ResetChairPosRot();
}
void ModelTestScene::ResetChairPosRot()
{
	m_pChair->GetTransform()->Translate(DirectX::XMVECTOR{ 0.f, 10.1f, 0.f });
	m_pChair->GetTransform()->Rotate(DirectX::XMFLOAT3{ 10.f * RandomSign() , 10.f * RandomSign(), 10.f * RandomSign() }, true);
}

ImVec4 ModelTestScene::LerpColors(const ImVec4& colorA, const ImVec4& colorB, float t)
{
	ImVec4 lerpedColor;
	lerpedColor.x = colorA.x + t * (colorB.x - colorA.x);
	lerpedColor.y = colorA.y + t * (colorB.y - colorA.y);
	lerpedColor.z = colorA.z + t * (colorB.z - colorA.z);
	lerpedColor.w = colorA.w + t * (colorB.w - colorA.w);

	return lerpedColor;
}

