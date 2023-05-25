#include "stdafx.h"
#include "PortalTestScene.h"
#include "Base\Logger.h"
#include <regex>
#include "Prefabs/Character.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Portal/PortalMaterial.h"
#include "Components\PortalComponent.h"
#include "Prefabs\SpherePrefab.h"
#include <filesystem>
void PortalTestScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	pBall1 = AddChild(new SpherePrefab(0.5f, 10, XMFLOAT4(Colors::Orange)));
	pBall2 = AddChild(new SpherePrefab(0.5f, 10, XMFLOAT4(Colors::Blue)));

	//Create portals
	CreatePortals();

	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	//GameSceneExt::CreatePhysXGroundPlane(*this, pDefaultMaterial);

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;

	m_pCharacter = AddChild(new Character(characterDesc));
	m_pCharacter->GetTransform()->Translate(0.f, 5.f, 15.f);

	////Simple Level
	//const auto pLevelObject = AddChild(new GameObject());
	//const auto pLevelMesh = pLevelObject->AddComponent(new ModelComponent(L"Labs/Week7/Meshes/SimpleLevel.ovm"));
	//pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());


	//const auto pLevelActor = pLevelObject->AddComponent(new RigidBodyComponent(true));
	//const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Labs/Week7/Meshes/SimpleLevel.ovpt");
	//pLevelActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ .5f,.5f,.5f })), *pDefaultMaterial);
	//pLevelObject->GetTransform()->Scale(.5f, .5f, .5f);

	// portal map

	LoadMap();

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);
}
void PortalTestScene::LoadMap()
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	const auto pMapObject = AddChild(new GameObject());

	std::wstring mapname = L"chamber02_static";

	const auto pMapMesh = pMapObject->AddComponent(new ModelComponent(L"Meshes/Maps/" + mapname + L".ovm"));

	LoadMapTextures(mapname, pMapMesh);

	//mapname = L"chamber_02_details";

	//const auto pMapMeshDetails = pMapObject->AddComponent(new ModelComponent(L"Meshes/Maps/" + mapname + L".ovm"));

	//LoadMapTextures(mapname, pMapMeshDetails);

	const auto pMapActor = pMapObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Maps/chamber_02_collision2.ovpt");
	pMapActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ 1,1,1 })), *pDefaultMaterial);

	m_pMap = pMapObject;
}

//namespace fs = std::filesystem;
//fs::path PortalTestScene::SearchFiles(fs::path directory, std::wregex pattern)
//{
//	if (fs::exists(directory) && fs::is_directory(directory))
//	{
//		for (const auto& entry : fs::directory_iterator(directory))
//		{
//			std::wsmatch match;
//			const std::wstring& path = entry.path().filename().wstring();
//			if (std::regex_search(path, match, pattern))
//			{
//				return entry.path();
//			}
//		}
//	}
//	return fs::path();
//}
//
//void PortalTestScene::LoadMapTextures(const std::wstring& mapname, ModelComponent* pMapModel)
//{
//	const auto pMeshFilter = ContentManager::Load<MeshFilter>(L"Meshes/Maps/" + mapname + L".ovm");
//	pMeshFilter->BuildIndexBuffer(m_SceneContext);
//
//	const auto meshes = pMeshFilter->GetMeshes();
//	std::vector<std::wstring> meshNames;
//	UINT8 index = 0;
//	for (const auto submesh : meshes)
//	{
//		meshNames.push_back(submesh.name);
//
//		// every submesh has a bunch of garbage in front of it as an artifact of having to use .ovm files instead of .fbx
//		// we gotta get rid of that garbage
//		std::wregex submeshRegex(L"^.*/");
//
//		// the last part of the submesh name holds the information regarding its material
//		// here we remove everything from the submesh.name except for the material name
//		std::wstring materialName = std::regex_replace(submesh.name, submeshRegex, L"");
//
//		// now we are going to through the chamber02 texture folder using the filsystem library.
//		// each texture file once again has a bunch of garbage in front of it, so we need to go through 
//		// each texture file with regex and check if the material name is in the texture file name.
//		// THIS IS VERY SLOW, but it is soooooooooooooooo much better than hardcoding the texture file names.
//		fs::path filePath = SearchFiles(fs::path(L"Resources/Textures/Maps/chamber02_static"), std::wregex(materialName));
//		//std::wstring filePath = L"Textures/Maps/chamber02_static/" + textureFileName;
//		//std::wstring absolutePath = L"Resources/" + filePath;
//
//		if (!std::filesystem::exists(filePath) || filePath.empty())
//		{
//			Logger::LogWarning(L"File path {} does not exist - mesh name: {}", filePath.wstring(), submesh.name);
//			filePath = L"Resources/Textures/Dev/dev_generic_orange.dds";
//		}
//
//		DiffuseMaterial* pDiffuse = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
//
//		std::filesystem::path base_path = "Resources";
//
//		std::filesystem::path relative_path = filePath.lexically_relative(base_path);
//
//		pDiffuse->SetDiffuseTexture(relative_path);
//		pMapModel->SetMaterial(pDiffuse, index);
//
//		++index;
//	}
//}

/**
* @brief Namespace alias for std::filesystem
*/
namespace fs = std::filesystem;

/**
 * @brief Searches for a file in a given directory that matches the provided pattern.
 * @param searchDirectory The directory where the search will be performed.
 * @param filePattern A regular expression that a file name must match to be considered a match.
 * @return The path of the first file that matches the pattern. If no such file is found, returns an empty path.
 */
fs::path PortalTestScene::SearchForMatchingFile(fs::path searchDirectory, std::wregex filePattern)
{
	if (fs::exists(searchDirectory) && fs::is_directory(searchDirectory))
	{
		for (const auto& directoryEntry : fs::directory_iterator(searchDirectory))
		{
			std::wsmatch match;
			const std::wstring& fileName = directoryEntry.path().filename().wstring();
			if (std::regex_search(fileName, match, filePattern))
			{
				return directoryEntry.path();
			}
		}
	}
	return fs::path();
}


// LoadMapTextures is very unoptimized but it works, it just ... works.
// in this function we get all the submesh names and perform regex searches on them.
// by doing this we get the material type that the submesh should have.
// then we perform another regex search through the texture folder to find the texture file that matches the material type.
// then we do some filesystem stuff to get the correct path to the texture file.

/**
 * @brief Loads the map textures for the given map name.
 * @param mapName The name of the map to load the textures for.
 * @param pMapModel Pointer to the model component where the loaded textures will be stored.
 */
void PortalTestScene::LoadMapTextures(const std::wstring& mapName, ModelComponent* pMapModel)
{

	// I need to get each submesh name and this is the only way I know how to do it
	const auto pMeshFilter = ContentManager::Load<MeshFilter>(L"Meshes/Maps/" + mapName + L".ovm");
	pMeshFilter->BuildIndexBuffer(m_SceneContext);

	const auto meshList = pMeshFilter->GetMeshes();
	UINT8 index = 0;
	for (const auto meshEntry : meshList)
	{

		std::wregex meshNameRegex(L"^.*/");
		 
		std::wstring materialName = std::regex_replace(meshEntry.name, meshNameRegex, L"");


		fs::path filePath = SearchForMatchingFile(fs::path(L"Resources/Textures/Maps/" + mapName), std::wregex(materialName));

		if (!std::filesystem::exists(filePath) || filePath.empty())
		{
			Logger::LogWarning(L"File path {} does not exist - mesh name: {}", filePath.wstring(), meshEntry.name);
			filePath = L"Resources/Textures/Dev/dev_generic_orange.dds";
		}


		DiffuseMaterial* pDiffuseMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();

		std::filesystem::path basePath = "Resources";

		std::filesystem::path relativePath = filePath.lexically_relative(basePath);

		pDiffuseMaterial->SetDiffuseTexture(relativePath);
		pMapModel->SetMaterial(pDiffuseMaterial, index);

		++index;
	}
}
//
//void PortalTestScene::InitializePortal(Portal portal)
//{
//	m_pPortals[portal] = AddChild(new GameObject());
//
//	const auto pLevelMesh = m_pPortals[portal]->AddComponent(new ModelComponent(L"Blender/Portal.ovm"));
//	pLevelMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());
//}
//std::wstring PortalTestScene::ConstructTextureFileName(const std::wstring& submeshName) const
//{
//	// Construct regex to match submesh pattern and extract relevant parts
//	//std::wregex submeshRegex(L"worldspawn_1_materials/(.*)");
//	std::wregex submeshRegex(L"(worldspawn_1_)?materials/(.*)");
//	std::wsmatch match;
//
//	if (std::regex_match(submeshName, match, submeshRegex))
//	{
//		// Replace slashes with underscores in the extracted part
//		std::wstring textureNamePart = std::regex_replace(match[2].str(), std::wregex(L"/"), L"_");
//
//		// Construct and return texture file name
//		return L"materials_" + textureNamePart + L".dds";
//	}
//	else
//	{
//		// Return empty string if submeshName did not match the pattern
//		return L"";
//	}
//}



//fs::path PortalTestScene::ConstructTextureFileName(const std::wstring& submeshName) const
//{
//	// Construct regex to match submesh pattern and extract relevant parts
//	//std::wregex submeshRegex(L"worldspawn_1_materials/(.*)");
//	std::wregex submeshRegex(L"^.*/");
//
//	std::wstring materialName = std::regex_replace(submeshName, submeshRegex, L"");
//
//	if (materialName.empty())
//		return  fs::path();
//
//	//std::wstring textureNamePart = std::regex_replace(match[2].str(), std::wregex(L"/"), L"_");
//	std::wregex pattern(materialName);
//
//	return search_files(materialName);
//}

void PortalTestScene::CreatePortals()
{
	//create game objects where portals willl be stored
	m_pPortals[Portal::Orange] = AddChild(new GameObject());
	m_pPortals[Portal::Blue] = AddChild(new GameObject());

	// add the portal logic to the game objects
	for (GameObject* pPortal : m_pPortals)
	{
		pPortal->AddComponent(new PortalComponent());

		const auto plevelmesh = pPortal->AddComponent(new ModelComponent(L"blender/portal.ovm"));
		plevelmesh->SetMaterial(MaterialManager::Get()->CreateMaterial<PortalMaterial>());

		// add rigidbody
		RigidBodyComponent* pRigidBody = pPortal->AddComponent(new RigidBodyComponent(true));
		PxBoxGeometry colliderSize
		{
			2,
			1,
			0.1f
		};

		auto* pMaterial = PxGetPhysics().createMaterial(.0f, .0f, 0.0f);

		pRigidBody->AddCollider(PxBoxGeometry{ colliderSize }, *pMaterial, false, { 0,1,0 });

	}

	// link PortalComponent of each game object to each other
	PortalComponent* orangePortal = m_pPortals[Portal::Orange]->GetComponent<PortalComponent>();
	PortalComponent* bluePortal = m_pPortals[Portal::Blue]->GetComponent<PortalComponent>();

	orangePortal->SetLinkedPortal(bluePortal);
	bluePortal->SetLinkedPortal(orangePortal);
}

void PortalTestScene::MovePortal(Portal portal)
{
	if (portal >= PortalsCount || portal < 0)
	{
		Logger::LogError(L"{} is not a valid Portal ID", (int)portal);
		assert(false);
	}

	//if (!m_pPortals[portal])
	//	InitializePortal(portal);

	m_pPortals[portal]->GetTransform()->Translate(m_pCharacter->GetTransform()->GetPosition());
	XMVECTOR playerRotation = XMLoadFloat4(&m_pCharacter->GetTransform()->GetRotation());
	m_pPortals[portal]->GetTransform()->Rotate(playerRotation);
}


void PortalTestScene::OnGUI()
{
	m_pCharacter->DrawImGui();

	static float scale = 1.f;

	ImGui::Begin("PortalTestScene");

	ImGui::DragFloat("Scale", &scale, 0.1f, 0.1f, 2.f);

	m_pMap->GetTransform()->Scale(scale);

	ImGui::End();
}

void PortalTestScene::Update()
{
	if (m_SceneContext.pInput->IsMouseButton(InputState::pressed, VK_RBUTTON))
	{
		MovePortal(Blue);
	}
	if (m_SceneContext.pInput->IsMouseButton(InputState::pressed, VK_XBUTTON1))
	{
		MovePortal(Orange);
	}

	TransformComponent* pCamera1, * pCamera2;

	pCamera1 = m_pPortals[Portal::Orange]->GetComponent<PortalComponent>()->GetPortalCamera()->GetTransform();
	pCamera2 = m_pPortals[Portal::Blue]->GetComponent<PortalComponent>()->GetPortalCamera()->GetTransform();

	pBall1->GetTransform()->Translate(pCamera1->GetPosition());
	pBall2->GetTransform()->Translate(pCamera2->GetPosition());
}