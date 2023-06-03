#include "stdafx.h"
#include "MapLoader.h"
#include <regex>
#include "Materials\DiffuseMaterial.h"
#include "Materials\Portal\DecalMaterial.h"
#include "Materials\Portal\StaticMapMaterial.h"
#include "Materials\Portal\PhongMaterial.h"
#include "Materials\ColorMaterial.h"
#include "Materials\Portal\NoDrawMaterial.h"
#include "ThreadPool.h"

MapLoader::MapLoader(GameScene& gamescene)
	:m_Scene(gamescene)
{
}

GameObject* MapLoader::LoadMap(const std::wstring& mapName)
{
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	const auto pMapObject = m_Scene.AddChild(new GameObject());

	const auto pMapMesh = pMapObject->AddComponent(new ModelComponent(L"Meshes/Maps/" + mapName + L".ovm"));
#ifdef _DEBUG
	LoadMapTexturesDebug(mapName, pMapMesh);
#else
	LoadMapTexturesRelease(mapName, pMapMesh);
#endif


	const auto pMapActor = pMapObject->AddComponent(new RigidBodyComponent(true));
	const auto pPxTriangleMesh = ContentManager::Load<PxTriangleMesh>(L"Meshes/Maps/chamber_02_collision2.ovpt");
	pMapActor->AddCollider(PxTriangleMeshGeometry(pPxTriangleMesh, PxMeshScale({ 1,1,1 })), *pDefaultMaterial);

	return pMapObject;
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
	const auto pMeshFilter = ContentManager::Load<MeshFilter>(L"Meshes/Maps/" + mapName + L".ovm");
	pMeshFilter->BuildIndexBuffer(m_Scene.GetSceneContext());

	const auto meshList = pMeshFilter->GetMeshes();
	UINT8 submeshID = 0;

	std::vector<std::future<SubmeshShaderInfo>> shaderInfoFutures;

	fs::path mapPath(L"Resources/Textures/Maps/" + mapName);
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
	const auto pMeshFilter = ContentManager::Load<MeshFilter>(L"Meshes/Maps/" + mapName + L".ovm");
	pMeshFilter->BuildIndexBuffer(m_Scene.GetSceneContext());

	const auto meshList = pMeshFilter->GetMeshes();
	UINT8 submeshID = 0;

	std::vector<SubmeshShaderInfo> submeshShaderInfoList;

	fs::path mapPath(L"Resources/Textures/Maps/" + mapName);
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
	pNoDrawMaterial->SetDebugRendering(false);
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
	std::wregex myRegex{L"elevatorshaft_wal"};
	if (std::regex_search(submeshName, myRegex))
	{
		return ShaderType::diffuse;
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
