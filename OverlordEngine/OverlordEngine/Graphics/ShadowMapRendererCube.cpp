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
	int lightNumber = 0;
	for (std::unique_ptr<ShadowMapCube>& pMap : m_ShadowCubes) 
	{
		m_NearPlane = pMap->GetNearPlane();
		m_FarPlane = pMap->GetFarPlane();
		pMap->Begin(sceneContext, lightNumber);
		++lightNumber;
	}
	ChangeViewportDimensions(static_cast<float>(m_CubemapResolution), static_cast<float>(m_CubemapResolution));
}

void ShadowMapRendererCube::DrawMesh(const SceneContext& sceneContext, int face,MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones)
{
	TODO_W8(L"Implement DrawMesh")
		for (std::unique_ptr<ShadowMapCube>& pMap : m_ShadowCubes) 
		{
			m_NearPlane = pMap->GetNearPlane();
			m_FarPlane = pMap->GetFarPlane();
			pMap->DrawMesh(sceneContext, face, pMeshFilter, meshWorld, meshBones);
		}
}

void ShadowMapRendererCube::End(const SceneContext& sceneContext) const
{
	ChangeViewportDimensions(static_cast<const float>(m_GameContext.windowWidth), static_cast<const float>(m_GameContext.windowHeight));

	// honestly just doing the commented line below will do the trick but I will do the other for
	// clarity.
	//m_GameContext.pGame->SetRenderTarget(nullptr);
	for (const std::unique_ptr<ShadowMapCube>& pMap : m_ShadowCubes)
		pMap->End(sceneContext);
}

void ShadowMapRendererCube::AddShadowMap(GameScene* pScene, float nearPlane, float farPlane)
{
	auto& shadowcube = m_ShadowCubes.emplace_back(std::make_unique<ShadowMapCube>(m_GameContext, m_CubemapResolution, nearPlane, farPlane));
	m_ShadowCubemapsCache.emplace(pScene, shadowcube->GetShadowMap());

}
// NEVER USE THIS FUNCTION IT IS NOT COMPLETE
//void ShadowMapRendererCube::RemoveShadowMap(UINT index)
//{
//	if (index < m_ShadowCubes.size())
//	{
//		m_ShadowCubes.erase(m_ShadowCubes.begin() + index);
//	}
//}

ID3D11ShaderResourceView* ShadowMapRendererCube::GetShadowMap(UINT index) const
{
	return m_ShadowCubes.at(index)->GetShadowMap();
}

std::vector<ID3D11ShaderResourceView*>& ShadowMapRendererCube::GetAllShadowCubemaps()
{
	GameScene* pScene = SceneManager::Get()->GetActiveScene();

	static GameScene* pPreviousScene{nullptr};
	if (pScene == pPreviousScene) return m_ShadowCubemapsSceneCache;

	pPreviousScene = pScene;

	m_ShadowCubemapsSceneCache.clear();
	auto range = m_ShadowCubemapsCache.equal_range(pScene);
	for (auto it = range.first; it != range.second; ++it)
	{
		m_ShadowCubemapsSceneCache.push_back(it->second);
	}

	return m_ShadowCubemapsSceneCache;
}

void ShadowMapRendererCube::Debug_DrawDepthSRV(UINT index,const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	return m_ShadowCubes.at(index)->Debug_DrawDepthSRV(position, scale,pivot);

}
