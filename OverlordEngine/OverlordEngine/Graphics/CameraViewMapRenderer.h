#pragma once
class CameraViewMapMaterial;
class CameraComponent;
class GameScene;
class CameraViewMapRenderer: public Singleton<CameraViewMapRenderer>
{
public:
	CameraViewMapRenderer(const CameraViewMapRenderer& other) = delete;
	CameraViewMapRenderer(CameraViewMapRenderer&& other) noexcept = delete;
	CameraViewMapRenderer& operator=(const CameraViewMapRenderer& other) = delete;
	CameraViewMapRenderer& operator=(CameraViewMapRenderer&& other) noexcept = delete;

	//void UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const;

	void Begin(const SceneContext&, CameraComponent* pCamera);
	void DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones = {});
	void End(const SceneContext&) const;

	ID3D11ShaderResourceView* GetColorMap() const;
	const XMFLOAT4X4& GetCameraVP() const { return m_CameraVP; }

	void Debug_DrawDepthSRV(const XMFLOAT2& position = { 0.f,0.f }, const XMFLOAT2& scale = { 1.f,1.f }, const XMFLOAT2& pivot = {0.f,0.f}) const;

protected:
	void Initialize() override;

private:
	friend class Singleton<CameraViewMapRenderer>;
	CameraViewMapRenderer() = default;
	~CameraViewMapRenderer();

	//Rendertarget to render the 'shadowmap' to (depth-only)
	//Contains depth information for all rendered shadow-casting meshes from a light's perspective (usual the main directional light)
	RenderTarget* m_pCameraRenderTarget{ nullptr };

	//Light ViewProjection (perspective used to render ShadowMap)
	XMFLOAT4X4 m_CameraVP{};
	CameraComponent* m_pCameraContainer;
	GameScene* m_pCurrentScene;

	//Shadow Generator is responsible of drawing all shadow casting meshes to the ShadowMap
	//There are two techniques, one for static (non-skinned) meshes, and another for skinned meshes (with bones, blendIndices, blendWeights)

	CameraViewMapMaterial* m_pCameraViewMapGenerator{ nullptr };

	//Information about each technique (static/skinned) is stored in a MaterialTechniqueContext structure
	//This information is automatically create by the Material class, we only store it in a local array for fast retrieval 
	MaterialTechniqueContext m_GeneratorTechniqueContext;

	inline static int m_InstanceCounter;
	int m_InstanceID;
};

