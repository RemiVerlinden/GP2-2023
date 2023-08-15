#pragma once
#include "Graphics\ShadowMapCube.h"

class ShadowMapRendererCube: public Singleton<ShadowMapRendererCube>
{
public:
	ShadowMapRendererCube(const ShadowMapRendererCube& other) = delete;
	ShadowMapRendererCube(ShadowMapRendererCube&& other) noexcept = delete;
	ShadowMapRendererCube& operator=(const ShadowMapRendererCube& other) = delete;
	ShadowMapRendererCube& operator=(ShadowMapRendererCube&& other) noexcept = delete;

	void UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const;

	void Begin(const SceneContext&);
	void DrawMesh(const SceneContext& sceneContext, int face, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones = {});
	void End(const SceneContext&);

	void AddShadowMap(GameScene* pScene, const Light& light);
	//void RemoveShadowMap(UINT index);

	ID3D11ShaderResourceView* GetShadowMap(UINT index) const;
	std::vector<ID3D11ShaderResourceView*>& GetAllShadowCubemaps();
	std::pair<std::vector<float>, std::vector<float>>& GetAllNearFarPlanes();
	std::vector<float>& GetAllPCFLevels();

	void Debug_DrawDepthSRV(UINT index, const XMFLOAT2& position = { 0.f,0.f }, const XMFLOAT2& scale = { 1.f,1.f }, const XMFLOAT2& pivot = {0.f,0.f}) const;
	ShadowMapMaterial* GetShadowMapGenerator() const { return m_pShadowMapGenerator; };

	enum class ShadowGeneratorType
	{
		Static,
		Skinned,
		Count
	};
protected:
	void Initialize() override;

private:
	friend class Singleton<ShadowMapRendererCube>;
	ShadowMapRendererCube() = default;
	~ShadowMapRendererCube();
	//void RegenerateShadowMapCache();

	void ChangeViewportDimensions(const float width, const float height) const;

	// ALL THESE VARIABLE ARE ALSO STORED IN SHADOWMAPCUBE CLASS BUT I PUT THEM HERE FOR CACHE BECAUSE I NEED THEM ALL EACH FRAME MULTIPLE TIMES
	std::unordered_map<GameScene*, std::vector<std::unique_ptr<ShadowMapCube>>> m_ShadowCubes;
	std::unordered_map<GameScene*, std::vector<ID3D11ShaderResourceView*>> m_ShadowCubemapsCache;
	std::unordered_map<GameScene*, std::pair<std::vector<float>, std::vector<float>>> m_CubemapsNearFarPlaneCache; // each scene has a pair of vectors, one for near planes and one for far planes
	std::unordered_map<GameScene*,std::vector<float>> m_LightPCFLevelsCache; // each scene has a pair of vectors, one for near planes and one for far planes
	//=======================================================================================================================================================================
	// 
	//Shadow Generator is responsible of drawing all shadow casting meshes to the ShadowMap
//There are two techniques, one for static (non-skinned) meshes, and another for skinned meshes (with bones, blendIndices, blendWeights)


	ShadowMapMaterial* m_pShadowMapGenerator{ nullptr };

	//Information about each technique (static/skinned) is stored in a MaterialTechniqueContext structure
	//This information is automatically create by the Material class, we only store it in a local array for fast retrieval 
	static int const NUM_TYPES{ 2 };
	MaterialTechniqueContext m_GeneratorTechniqueContexts[NUM_TYPES];


	// camera info
	float m_CubemapResolution{512};
};

