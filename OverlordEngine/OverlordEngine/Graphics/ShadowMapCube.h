#pragma once
class ShadowMapMaterial;

class ShadowMapCube
{
public:
    ShadowMapCube(const GameContext& gamecontext, float cubemapResolution, float nearPlane, float farPlane);
    ~ShadowMapCube();

    void Begin(const SceneContext&, int lightNumber);
    void DrawMesh(const SceneContext& sceneContext, int face, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones = {});
    void End(const SceneContext&);

    ID3D11ShaderResourceView* GetShadowMap() const;
    void Debug_DrawDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const;

    float GetNearPlane() const { return m_NearPlane; };
    float GetFarPlane() const { return m_FarPlane; };

private:
    const GameContext& m_GameContext;
    // Private members for individual ShadowMapCube here...
    // These might be similar to what's currently in ShadowMapRendererCube
    // for a single shadow map.
    RenderTarget* m_pShadowRenderTarget{ nullptr };
    std::vector<XMFLOAT4X4> m_LightVP;

    int m_LightNumber{-1};

    float m_NearPlane{}, m_FarPlane{};
    GameScene* m_pScene{};
};