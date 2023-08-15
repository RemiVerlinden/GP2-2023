#include "stdafx.h"
#include "ShadowMapRendererCube.h"
#include "Misc/ShadowMapMaterial.h"

ShadowMapRendererCube::~ShadowMapRendererCube()
{
}


void ShadowMapRendererCube::Initialize()
{
	m_pShadowMapGenerator = MaterialManager::Get()->CreateMaterial<ShadowMapMaterial>(); // CUBE MAP

	for (int ID = 0; ID < (int)ShadowGeneratorType::Count; ++ID)
		m_GeneratorTechniqueContexts[ID] = m_pShadowMapGenerator->GetTechniqueContext(ID);
}

void ShadowMapRendererCube::UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const
{
	TODO_W8(L"Implement UpdateMeshFilter")
		//Here we want to Update the MeshFilter of ModelComponents that need to be rendered to the ShadowMap
		//Updating the MeshFilter means that we want to create a corresponding VertexBuffer for our ShadowGenerator material

		//1. Figure out the correct ShadowGeneratorType (either Static, or Skinned) with information from the incoming MeshFilter
		ShadowGeneratorType meshType = pMeshFilter->HasAnimations() ? ShadowGeneratorType::Skinned : ShadowGeneratorType::Static;

	//2. Retrieve the corresponding TechniqueContext from m_GeneratorTechniqueContexts array (Static/Skinned)
	const MaterialTechniqueContext& context = m_GeneratorTechniqueContexts[(int)meshType];

	//3. Build a corresponding VertexBuffer with data from the relevant TechniqueContext you retrieved in Step2 (MeshFilter::BuildVertexBuffer)
	pMeshFilter->BuildVertexBuffer(sceneContext, context.inputLayoutID, context.inputLayoutSize, context.pInputLayoutDescriptions);
}

// we must change the vieort dimensions to match the cubemap resolution else when we render the shadowmap it will be stretched
void ShadowMapRendererCube::ChangeViewportDimensions(const float width, const float height) const
{
	D3D11_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	m_GameContext.d3dContext.pDeviceContext->RSSetViewports(1, &viewport);
}

void ShadowMapRendererCube::Begin(const SceneContext& sceneContext)
{
	GameScene* pScene = SceneManager::Get()->GetActiveScene();

	int lightNumber = 0;
	for (std::unique_ptr<ShadowMapCube>& pMap : m_ShadowCubes.at(pScene))
	{
		pMap->Begin(sceneContext, lightNumber);
		++lightNumber;
	}
	ChangeViewportDimensions(static_cast<float>(m_CubemapResolution), static_cast<float>(m_CubemapResolution));
}

void ShadowMapRendererCube::DrawMesh(const SceneContext& sceneContext, int face, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones)
{
	GameScene* pScene = SceneManager::Get()->GetActiveScene();

	for (std::unique_ptr<ShadowMapCube>& pMap : m_ShadowCubes.at(pScene))
	{
		pMap->DrawMesh(sceneContext, face, pMeshFilter, meshWorld, meshBones);
	}
}

void ShadowMapRendererCube::End(const SceneContext& sceneContext) 
{
	ChangeViewportDimensions(static_cast<const float>(m_GameContext.windowWidth), static_cast<const float>(m_GameContext.windowHeight));

	// honestly just doing the commented line below will do the trick but I will do the other for
	// clarity.
	//m_GameContext.pGame->SetRenderTarget(nullptr);
	GameScene* pScene = SceneManager::Get()->GetActiveScene();

	for (std::unique_ptr<ShadowMapCube>& pMap : m_ShadowCubes.at(pScene))
		pMap->End(sceneContext);
}


// all this complicated code just for stupid caching
void ShadowMapRendererCube::AddShadowMap(GameScene* pScene, const Light& light)
{
	m_LightPCFLevelsCache[pScene].emplace_back(static_cast<float>(light.PCFLevel));

	//if (m_CubemapsNearFarPlaneCache.find(pScene) == m_CubemapsNearFarPlaneCache.end())
	//	m_CubemapsNearFarPlaneCache[pScene] = std::pair<std::vector<float>, std::vector<float>>(); // This step is technically optional, because the map will auto-create an empty vector, but it makes things explicit.

	m_CubemapsNearFarPlaneCache[pScene].first.emplace_back(light.nearPlane);
	m_CubemapsNearFarPlaneCache[pScene].second.emplace_back(light.farPlane);

	//if (m_ShadowCubes.find(pScene) == m_ShadowCubes.end())
		//m_ShadowCubes[pScene] = std::vector<std::unique_ptr<ShadowMapCube>>(); // This step is technically optional, because the map will auto-create an empty vector, but it makes things explicit.

	auto& shadowcube = m_ShadowCubes[pScene].emplace_back(std::make_unique<ShadowMapCube>(m_GameContext, m_CubemapResolution, light));


	//if (m_ShadowCubemapsCache.find(pScene) == m_ShadowCubemapsCache.end())
		//m_ShadowCubemapsCache[pScene] = std::vector<ID3D11ShaderResourceView*>();

	m_ShadowCubemapsCache[pScene].push_back(shadowcube->GetShadowMap());
}

// check what scene we are in then get the shadowmap at that index
ID3D11ShaderResourceView* ShadowMapRendererCube::GetShadowMap(UINT index) const
{
	GameScene* pScene = SceneManager::Get()->GetActiveScene();
	return m_ShadowCubes.at(pScene).at(index)->GetShadowMap();
}

std::vector<ID3D11ShaderResourceView*>& ShadowMapRendererCube::GetAllShadowCubemaps()
{
	GameScene* pScene = SceneManager::Get()->GetActiveScene();

	if (m_ShadowCubemapsCache.find(pScene) == m_ShadowCubemapsCache.end())
	{
		Logger::LogError(L"Trying to get shadowmaps from a scene that doesnt have any( this means you did not add any lights therefore no shadowmaps)");
		static std::vector<ID3D11ShaderResourceView*> emptyVector;
		return emptyVector;
	}

	return m_ShadowCubemapsCache[pScene];
}

std::pair<std::vector<float>, std::vector<float>>& ShadowMapRendererCube::GetAllNearFarPlanes()
{
	GameScene* pScene = SceneManager::Get()->GetActiveScene();

	if (m_CubemapsNearFarPlaneCache.find(pScene) == m_CubemapsNearFarPlaneCache.end())
	{
		Logger::LogError(L"Trying to get shadowmaps from a scene that doesnt have any( this means you did not add any lights therefore no shadowmaps)");
		static std::pair<std::vector<float>, std::vector<float>> emptyVector;
		return emptyVector;
	}

	return m_CubemapsNearFarPlaneCache[pScene];
}

std::vector<float>& ShadowMapRendererCube::GetAllPCFLevels()
{
	GameScene* pScene = SceneManager::Get()->GetActiveScene();

	if (m_LightPCFLevelsCache.find(pScene) == m_LightPCFLevelsCache.end())
	{
		Logger::LogError(L"Trying to get shadowmaps from a scene that doesnt have any( this means you did not add any lights therefore no shadowmaps)");
		static std::vector<float> emptyVector;
		return emptyVector;
	}

	return m_LightPCFLevelsCache[pScene];
}

void ShadowMapRendererCube::Debug_DrawDepthSRV(UINT index, const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	GameScene* pScene = SceneManager::Get()->GetActiveScene();
	return m_ShadowCubes.at(pScene).at(index)->Debug_DrawDepthSRV(position, scale, pivot);
}
