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
	void End(const SceneContext&) const;

	void AddShadowMap(GameScene* pScene, float nearPlane, float farPlane);
	//void RemoveShadowMap(UINT index);

	ID3D11ShaderResourceView* GetShadowMap(UINT index) const;
	std::vector<ID3D11ShaderResourceView*>& GetAllShadowCubemaps();
	static inline float GetNearPlane() { return m_NearPlane; };
	static inline float GetFarPlane() { return 20.f; };

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

	std::vector<std::unique_ptr<ShadowMapCube>> m_ShadowCubes;
	std::unordered_multimap<GameScene*, ID3D11ShaderResourceView*> m_ShadowCubemapsCache;
	std::vector<ID3D11ShaderResourceView*> m_ShadowCubemapsSceneCache;

	//Shadow Generator is responsible of drawing all shadow casting meshes to the ShadowMap
//There are two techniques, one for static (non-skinned) meshes, and another for skinned meshes (with bones, blendIndices, blendWeights)


	ShadowMapMaterial* m_pShadowMapGenerator{ nullptr };

	//Information about each technique (static/skinned) is stored in a MaterialTechniqueContext structure
	//This information is automatically create by the Material class, we only store it in a local array for fast retrieval 
	static int const NUM_TYPES{ 2 };
	MaterialTechniqueContext m_GeneratorTechniqueContexts[NUM_TYPES];


	// camera info
	static inline float m_NearPlane{ 0.01f };
	static inline float m_FarPlane{ 20.f };
	float m_CubemapResolution{512};
};

