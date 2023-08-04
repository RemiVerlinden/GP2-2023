#include "stdafx.h"
#include "MapLoader.h"
#include <regex>
#include "Materials\DiffuseMaterial.h"
#include "Materials\Portal\DecalMaterial.h"
#include "Materials\Portal\StaticMapMaterial.h"
#include "Materials\Portal\PhongMaterial.h"
#include "Materials\Portal\PhongMaterial_Skinned.h"
#include "Materials\Portal\Glass\FrostedGlassMaterial.h"
#include "Materials\Portal\Glass\RefractingGlassMaterial.h"
#include "Materials\ColorMaterial.h"
#include "Materials\Portal\NoDrawMaterial.h"
#include "ThreadPool.h"

MapLoader::MapLoader(GameScene& gamescene)
	:m_Scene(gamescene)
{
}

GameObject* MapLoader::LoadMap(const std::wstring& mapName)
{
	LoadDynamicProps(mapName);

	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	const auto pMapObject = m_Scene.AddChild(new GameObject());
	pMapObject->SetTag(L"LevelObject");

	const auto pMapMesh = pMapObject->AddComponent(new ModelComponent(L"Meshes/Maps/" + mapName + L"_static.ovm"));
#ifdef _DEBUG
	LoadMapTexturesDebug(mapName, pMapMesh);
#else
	LoadMapTexturesRelease(mapName, pMapMesh);
#endif


	const auto pMapRigidBody = pMapObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Maps/" + mapName + L"_collision2.ovpt");
	pMapRigidBody->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ 1,1,1 })), *pDefaultMaterial);

	pMapRigidBody->SetCollisionGroup(CollisionGroup::Group0 | CollisionGroup::Group4);

	return pMapObject;
}

MapLoader::InteractiveElements& MapLoader::GetInteractiveElements()
{
	return m_InteractiveElements;	
}


// in debug mode the "std::regex_search" is a literal olympic gold medalist for the "shit performance" category
// so I tried to optimise it with a thread pool (barely doubles the speed but is still something)
//
// also, there is no need for multithreading in the release version because it already runs nearly instantly
// and trying to optimise even further will probably just break my code no thank you to that
void MapLoader::LoadMapTexturesDebug(const std::wstring& mapName, ModelComponent* pMapModel)
{
	ThreadPool pool(4); // more than this pretty much doesnt help

	// I need to get each submesh name and this is the only way I know how to do it
	const auto pMeshFilter = ContentManager::Load<MeshFilter>(L"Meshes/Maps/" + mapName + L"_static.ovm");
	pMeshFilter->BuildIndexBuffer(m_Scene.GetSceneContext());

	const auto meshList = pMeshFilter->GetMeshes();
	UINT8 submeshID = 0;

	std::vector<std::future<SubmeshShaderInfo>> shaderInfoFutures;

	fs::path mapPath(L"Resources/Textures/Maps/" + mapName + L"_static");
	for (const auto submesh : meshList)
	{
		// this is all local info that I use as input for functions
		SubmeshShaderInfo shaderInfo;
		shaderInfo.pModel = pMapModel;
		shaderInfo.submeshID = submeshID;
		shaderInfo.pMapPath = &mapPath;

		// extract material name
		std::wregex meshNameRegex(L"^.*/");
		shaderInfo.materialName = std::regex_replace(submesh.name, meshNameRegex, L"");


		auto threadedLambda = [this](SubmeshShaderInfo shaderInfo, std::wstring submeshName) -> SubmeshShaderInfo
		{
			// find all textures that belong to the submesh (diffuse, normal, specular)
			FindTexurePaths(shaderInfo);

			// now we have all the info we need in order to find out what shader our submesh should use
			//(we have all the texture paths and an extensive submeshname)
			shaderInfo.shaderType = IdentifyShaderType(submeshName, shaderInfo);

			return shaderInfo;
		};

		shaderInfoFutures.push_back(pool.enqueue(threadedLambda, std::move(shaderInfo), submesh.name));

		++submeshID;
	}

	for (std::future<SubmeshShaderInfo>& shaderInfoFuture : shaderInfoFutures)
	{
		SubmeshShaderInfo shaderInfo{ std::move(shaderInfoFuture.get()) };
		switch (shaderInfo.shaderType)
		{
			case MapLoader::ShaderType::noDraw:
				LoadNoDrawShaderForSubmesh(shaderInfo);
				break;
			case MapLoader::ShaderType::decal:
				LoadDecalShaderForSubmesh(shaderInfo);
				break;
			case MapLoader::ShaderType::diffuse:
				LoadDiffuseShaderForSubmesh(shaderInfo);
				break;
			case MapLoader::ShaderType::PhongProp:
			case MapLoader::ShaderType::PhongBrush:
				LoadPhongShaderForSubmesh(shaderInfo);
				break;
			case  MapLoader::ShaderType::light:
				LoadLightShaderForSubmesh(shaderInfo);
				break;
			case  MapLoader::ShaderType::frostedGlass:
				LoadFrostedGlassShaderForSubmesh(shaderInfo);
				break;
			case  MapLoader::ShaderType::refractingGlass:
				LoadNoDrawShaderForSubmesh(shaderInfo);
				//LoadRefractingGlassShaderForSubmesh(shaderInfo);
				break;
		}
	}
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
void MapLoader::LoadMapTexturesRelease(const std::wstring& mapName, ModelComponent* pMapModel)
{
	// I need to get each submesh name and this is the only way I know how to do it
	const auto pMeshFilter = ContentManager::Load<MeshFilter>(L"Meshes/Maps/" + mapName + L"_static.ovm");
	pMeshFilter->BuildIndexBuffer(m_Scene.GetSceneContext());

	const auto meshList = pMeshFilter->GetMeshes();
	UINT8 submeshID = 0;

	std::vector<SubmeshShaderInfo> submeshShaderInfoList;

	fs::path mapPath(L"Resources/Textures/Maps/" + mapName + L"_static");
	for (const auto submesh : meshList)
	{
		// this is all local info that I use as input for functions
		SubmeshShaderInfo shaderInfo;
		shaderInfo.pModel = pMapModel;
		shaderInfo.submeshID = submeshID;
		shaderInfo.pMapPath = &mapPath;

		// extract material name
		std::wregex meshNameRegex(L"^.*/");
		shaderInfo.materialName = std::regex_replace(submesh.name, meshNameRegex, L"");

		// find all textures that belong to the submesh (diffuse, normal, specular)
		FindTexurePaths(shaderInfo);

		// now we have all the info we need in order to find out what shader our submesh should use
		//(we have all the texture paths and an extensive submeshname)
		shaderInfo.shaderType = IdentifyShaderType(submesh.name, shaderInfo);


		switch (shaderInfo.shaderType)
		{
			case MapLoader::ShaderType::noDraw:
				LoadNoDrawShaderForSubmesh(shaderInfo);
				break;
			case MapLoader::ShaderType::decal:
				LoadDecalShaderForSubmesh(shaderInfo);
				break;
			case MapLoader::ShaderType::diffuse:
				LoadDiffuseShaderForSubmesh(shaderInfo);
				break;
			case MapLoader::ShaderType::PhongProp:
			case MapLoader::ShaderType::PhongBrush:
				LoadPhongShaderForSubmesh(shaderInfo);
				break;
			case  MapLoader::ShaderType::light:
				LoadLightShaderForSubmesh(shaderInfo);
				break;
			case  MapLoader::ShaderType::frostedGlass:
				LoadFrostedGlassShaderForSubmesh(shaderInfo);
				break;
			case  MapLoader::ShaderType::refractingGlass:
				LoadNoDrawShaderForSubmesh(shaderInfo);
				//LoadRefractingGlassShaderForSubmesh(shaderInfo);
				break;
		}


		++submeshID;
	}
}

namespace
{
	/**
	 * @brief Searches for a file in a given directory that matches the provided pattern.
	 * @param searchDirectory The directory where the search will be performed.
	 * @param filePattern A regular expression that a file name must match to be considered a match.
	 * @return The path of the first file that matches the pattern. If no such file is found, returns an empty path.
	 */
	fs::path SearchForMatchingFile(fs::path searchDirectory, std::wregex filePattern)
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
}



void MapLoader::FindTexurePaths(SubmeshShaderInfo& shaderInfo)
{
	const fs::path& mapPath = *shaderInfo.pMapPath;
	const std::wstring& materialName = shaderInfo.materialName;


	// this doesnt have to be in a lambda but it feels more organized this way
	auto FindDiffuseMapPath = [&mapPath, &materialName]() -> fs::path
	{
		fs::path filePathDiffuse = SearchForMatchingFile(mapPath, std::wregex(materialName));

	
		if (!std::filesystem::exists(filePathDiffuse) || filePathDiffuse.empty())
		{
			Logger::LogWarning(L"File path {} does not exist - material name: {}", filePathDiffuse.wstring(), materialName);
			filePathDiffuse = L"Resources/Textures/Dev/dev_generic_orange.dds";
		}
		return filePathDiffuse;
	};

	// for normal and exponent maps
	auto FindAdditionalMapPath = [&mapPath, &materialName](const std::wstring& appendedMaterialName) -> fs::path
	{
		std::wstring normalMaterialName = std::format(L"{}{}", materialName, appendedMaterialName);
		fs::path filePathNormal = SearchForMatchingFile(mapPath, std::wregex(normalMaterialName));
		if (!filePathNormal.empty())
		{
			return filePathNormal;
		}
		return fs::path();
	};



	shaderInfo.diffusePath = FindDiffuseMapPath();
	shaderInfo.normalPath = FindAdditionalMapPath(L"_normal");
	shaderInfo.specularPath = FindAdditionalMapPath(L"_exponent");
}

void MapLoader::LoadNoDrawShaderForSubmesh(SubmeshShaderInfo& shaderInfo)
{
	NoDrawMaterial* pNoDrawMaterial = MaterialManager::Get()->CreateMaterial<NoDrawMaterial>();

#ifdef _DEBUG
	pNoDrawMaterial->SetDebugRendering(true);
#else
	pNoDrawMaterial->SetDebugRendering(false);
#endif

	shaderInfo.pModel->SetMaterial(pNoDrawMaterial, shaderInfo.submeshID);
}

void MapLoader::LoadDecalShaderForSubmesh(SubmeshShaderInfo& shaderInfo)
{
	DecalMaterial* pDecalMaterial = MaterialManager::Get()->CreateMaterial<DecalMaterial>();


	pDecalMaterial->SetDecalTexture(ShortenTexturePath(shaderInfo.diffusePath));
	shaderInfo.pModel->SetMaterial(pDecalMaterial, shaderInfo.submeshID);
}

void MapLoader::LoadDiffuseShaderForSubmesh(SubmeshShaderInfo& shaderInfo)
{
	StaticMapMaterial* pDiffuseMaterial = MaterialManager::Get()->CreateMaterial<StaticMapMaterial>();

	pDiffuseMaterial->SetDiffuseTexture(ShortenTexturePath(shaderInfo.diffusePath));
	shaderInfo.pModel->SetMaterial(pDiffuseMaterial, shaderInfo.submeshID);
}

void MapLoader::LoadPhongShaderForSubmesh(SubmeshShaderInfo& shaderInfo)
{
	PhongMaterial* pPhongMaterial = MaterialManager::Get()->CreateMaterial<PhongMaterial>();

	pPhongMaterial->SetDiffuseTexture(ShortenTexturePath(shaderInfo.diffusePath));
	pPhongMaterial->SetNormalTexture(ShortenTexturePath(shaderInfo.normalPath));

	//if (shaderInfo.shaderType == ShaderType::PhongProp)
	//	pPhongMaterial->SetSpecularTexture(ShortenTexturePath(shaderInfo.specularPath));

	shaderInfo.pModel->SetMaterial(pPhongMaterial, shaderInfo.submeshID);
}

void MapLoader::LoadLightShaderForSubmesh(SubmeshShaderInfo& shaderInfo)
{
	ColorMaterial* pLightMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();

	pLightMaterial->SetColor((XMFLOAT4)Colors::White);
	shaderInfo.pModel->SetMaterial(pLightMaterial, shaderInfo.submeshID);
}

void MapLoader::LoadFrostedGlassShaderForSubmesh(SubmeshShaderInfo& shaderInfo) 
{
	FrostedGlassMaterial* pFrostedGlassMaterial = MaterialManager::Get()->CreateMaterial<FrostedGlassMaterial>();
	pFrostedGlassMaterial->SetDiffuseTexture(ShortenTexturePath(shaderInfo.diffusePath));
	pFrostedGlassMaterial->SetOpacity(0.7f);

	// still need to look at the fx file and update it

	shaderInfo.pModel->SetMaterial(pFrostedGlassMaterial, shaderInfo.submeshID);

}
void MapLoader::LoadRefractingGlassShaderForSubmesh(SubmeshShaderInfo& shaderInfo) 
{
	RefractingGlassMaterial* pRefractingGlassMaterial = MaterialManager::Get()->CreateMaterial<RefractingGlassMaterial>();

	// dont know how to crash so will just use both.
	assert(true);
	assert(false);

	//still need to set the diffuse , normal, possibly specular and opacity
	// still need to look at the fx file and update it

	shaderInfo.pModel->SetMaterial(pRefractingGlassMaterial, shaderInfo.submeshID);
}


// this has to be called each time because when loading in a texture with the texture loader,
// we cant have the beginning "Rsources" part in the path.
// this is not because of my code, its how the engine was given to me so I have to work around it
fs::path MapLoader::ShortenTexturePath(const fs::path& texturePath)
{
	std::filesystem::path basePath = "Resources";

	return texturePath.lexically_relative(basePath);
}


// note, I could set the shadertype in straight up in this function instead of returning it
// but I feel like the code is easier to read if all of shaderInfo is initialized in one place [LoadMapTextures()]
MapLoader::ShaderType MapLoader::IdentifyShaderType(const std::wstring& submeshName, SubmeshShaderInfo& shaderInfo)
{
	// In the obj file of the level, I have all noDraw meshes in ID 0
	// but for some reason when the ovm level file is read, it decides to rename the noDraw to frosted glass.
	// so I just force the ID 0 to be noDraw
	if(shaderInfo.submeshID == 0) return ShaderType::noDraw; 

	std::wregex myRegex{L"elevatorshaft_wal"};
	if (std::regex_search(submeshName, myRegex))
	{
		return ShaderType::diffuse;
	}

	std::wregex searchRefractRegex{L"glasswindow_refract"};
	if (std::regex_search(submeshName, searchRefractRegex))
	{
		shaderInfo.diffusePath = fs::path(L"Resources/Textures/Maps/chamber02_static\\materials_glass_glasswindow_frosted.dds");
		return ShaderType::frostedGlass;
	}

	std::wregex searchFrostRegex{L"glasswindow_frosted"};
	if (std::regex_search(submeshName, searchFrostRegex))
	{
		return ShaderType::frostedGlass;
	}


	if (!shaderInfo.normalPath.empty())
	{
		if (!shaderInfo.specularPath.empty())
		{
			return ShaderType::PhongProp; // if we have a normal and specular map, are dealing with prop phong
		}
		return ShaderType::PhongBrush; // if we only have a normal then we are dealing with a brush phong
	}

	// no normal or specular present? then we are dealing with diffuse, light or decal
	const std::vector <std::pair< ShaderType, std::wregex >> shaderRegexMap =
	{
		{ ShaderType::noDraw, std::wregex(L"toolsnodraw", std::regex_constants::icase) },
		{ ShaderType::diffuse, std::wregex(L"(boxhurt | boxdispenser)", std::regex_constants::icase) },
		{ ShaderType::light, std::wregex(L"materials/lights", std::regex_constants::icase) },
		{ ShaderType::decal, std::wregex(L"(/decal|/overlay)", std::regex_constants::icase) },
		{ ShaderType::decal, std::wregex(L"metalgrate", std::regex_constants::icase) }
	};


	for (const auto& [shaderType, regex] : shaderRegexMap)
	{
		if (std::regex_search(submeshName, regex))
		{
			return shaderType;
		}
	}

	return ShaderType::diffuse;
}

void MapLoader::LoadDynamicProps(const std::wstring& mapName)
{
	DynamicPropsParseNames parserNames;
	// Prepare the map of names to functions
	std::map<std::wstring, void(MapLoader::*)(const XMFLOAT3&)> spawnFuncs = 
	{
		{parserNames.button, &MapLoader::SpawnButton},
		{parserNames.elevator, &MapLoader::SpawnElevator},
		{parserNames.door, &MapLoader::SpawnDoor},
		{parserNames.cube, &MapLoader::SpawnCube}
	};

	fs::path mapPath = L"Resources/Meshes/Maps/" + mapName + L"_dynamic-props.txt";
	std::wifstream file(mapPath);

	if (!file.is_open())
	{
		throw std::runtime_error("Could not open file");
	}

	std::wstring line;
	std::wstring itemName;
	while (std::getline(file, line))
	{
		if (line == L"END")
		{
			itemName = L"";
		}
		else
		{
			std::wstringstream ss(line);
			float x, y, z;
			if (ss >> x && ss >> y && ss >> z)
			{
				auto spawnFuncIt = spawnFuncs.find(itemName);
				if (spawnFuncIt != spawnFuncs.end())
				{
					(this->*spawnFuncIt->second)(XMFLOAT3(x, y, z));
				}
			}
			else
				itemName = line;
		}
	}
}


void MapLoader::SpawnButton(const XMFLOAT3& position)
{
	static ButtonProperties props;

	static PxMaterial* pMaterial = PxGetPhysics().createMaterial(props.pxMaterial[0], props.pxMaterial[1], props.pxMaterial[2]);


	auto pPhong = MaterialManager::Get()->CreateMaterial<PhongMaterial>();
	pPhong->SetDiffuseTexture(props.diffuseMapPath);
	pPhong->SetNormalTexture(props.normalMapPath);

	const auto pButton = m_Scene.AddChild(new GameObject());
	ModelComponent* pModel = pButton->AddComponent(new ModelComponent(props.modelPath));
	pModel->SetMaterial(pPhong);

	[[maybe_unused]]ButtonAnimComponent* pButtonAnim = pButton->AddComponent(new ButtonAnimComponent());

	pButton->GetTransform()->Translate(position);

	RigidBodyComponent* pButtonRigidBody = pButton->AddComponent(new RigidBodyComponent(false));
	pButtonRigidBody->SetKinematic(true);
	pButtonRigidBody->SetCollisionGroup(CollisionGroup::Group2);


	PxConvexMesh* pConvexMesh = ContentManager::Load<PxConvexMesh>(props.rigidBodyPath);
	PxConvexMeshGeometry buttonMeshGeometry{ pConvexMesh };
	pButtonRigidBody->AddCollider(buttonMeshGeometry, *pMaterial, false);

	// trigger for button
	RigidBodyComponent* pButtonTriggeRigidBody = pButton->AddComponent(new RigidBodyComponent(true));
	pButtonTriggeRigidBody->SetCollisionIgnoreGroups(CollisionGroup::Group2);

	buttonMeshGeometry.scale = PxMeshScale(1.06f); // just make the trigger a little bigger because I like it that way
	pButtonTriggeRigidBody->AddCollider(buttonMeshGeometry, *pMaterial, true, { 0,0.4f,0 }); // third parameter set to true for trigger
	pButton->SetOnTriggerCallBack([pButtonAnim](GameObject*, GameObject*, PxTriggerAction action)
	{
		static size_t objectInTrigger = 0;

		if (action == PxTriggerAction::ENTER)
		{
			++objectInTrigger;
			pButtonAnim->SetPressed(true);
		}
		if (action == PxTriggerAction::LEAVE)
		{
			--objectInTrigger;
			if (objectInTrigger == 0) pButtonAnim->SetPressed(false);
		}
	});
	

	m_InteractiveElements.buttons.emplace_back(pButton);
}

void MapLoader::SpawnElevator(const XMFLOAT3& /*position*/)
{
}

void MapLoader::SpawnDoor(const XMFLOAT3& position)
{
	static DoorProperties props;

	GameObject* pDoor = m_Scene.AddChild(new GameObject);
	pDoor->GetTransform()->Translate(position);
	pDoor->AddComponent(new DoorComponent());

	m_InteractiveElements.doors.push_back(pDoor);
}

void MapLoader::SpawnCube(const XMFLOAT3& position)
{
	static CubeProperties props;

	static PxMaterial* pMaterial = PxGetPhysics().createMaterial(props.pxMaterial[0], props.pxMaterial[1], props.pxMaterial[2]);
	auto pPhong = MaterialManager::Get()->CreateMaterial<PhongMaterial>();
	pPhong->SetDiffuseTexture(props.diffuseMapPath);
	pPhong->SetNormalTexture(props.normalMapPath);

	GameObject* pCube = m_Scene.AddChild(new GameObject);
	ModelComponent* pModelTop = pCube->AddComponent(new ModelComponent(props.modelPath));
	pModelTop->SetMaterial(pPhong);


	PxConvexMesh* pConvexMesh = ContentManager::Load<PxConvexMesh>(props.rigidBodyPath);
	RigidBodyComponent* pRigidBody = pCube->AddComponent(new RigidBodyComponent());
	pRigidBody->AddCollider(PxConvexMeshGeometry{ pConvexMesh }, *pMaterial, false);



	PxRigidBodyExt::updateMassAndInertia(*pRigidBody->GetPxRigidActor()->is<PxRigidBody>(), props.mass);

	pCube->GetTransform()->Translate(position);

	m_InteractiveElements.cubes.emplace_back(pCube);
}


