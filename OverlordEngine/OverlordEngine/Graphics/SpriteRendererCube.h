#pragma once
class SpriteRendererCube final : public Singleton<SpriteRendererCube>
{
public:
	SpriteRendererCube(const SpriteRendererCube& other) = delete;
	SpriteRendererCube(SpriteRendererCube&& other) noexcept = delete;
	SpriteRendererCube& operator=(const SpriteRendererCube& other) = delete;
	SpriteRendererCube& operator=(SpriteRendererCube&& other) noexcept = delete;

	void Draw(const SceneContext& sceneContext);

	void DrawImmediate(const D3D11Context& d3dContext, ID3D11ShaderResourceView* pSrv, const XMFLOAT2& position, const XMFLOAT4& color = XMFLOAT4{ Colors::White }, const XMFLOAT2& pivot = XMFLOAT2{ 0.f, 0.f }, const XMFLOAT2& scale = XMFLOAT2{ 1.f, 1.f }, float rotation = 0.f);

	void AppendSprite(TextureData* pTexture, const XMFLOAT2& position, const XMFLOAT4& color = XMFLOAT4{ Colors::White }, const XMFLOAT2& pivot = XMFLOAT2{ 0, 0 }, const XMFLOAT2& scale = XMFLOAT2{ 1, 1 }, float rotation = 0.f, float depth = 0.f);
protected:
	void Initialize() override;

private:
	friend class Singleton<SpriteRendererCube>;
	SpriteRendererCube() = default;
	~SpriteRendererCube();

	void UpdateBuffer(const SceneContext& sceneContext);

	std::vector<VertexSprite> m_Sprites{};
	std::vector<TextureData*> m_Textures{};
	UINT m_BufferSize{ 50 };
	UINT m_InputLayoutSize{};

	ID3DX11Effect* m_pEffect{};
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout{};

	ID3D11Buffer* m_pVertexBuffer{}, * m_pImmediateVertexBuffer{};
	VertexSprite m_ImmediateVertex{};

	XMFLOAT4X4 m_Transform{};
	ID3DX11EffectMatrixVariable* m_pEVar_TransformMatrix{};
	ID3DX11EffectVectorVariable* m_pEVar_TextureSize{};
	ID3DX11EffectShaderResourceVariable* m_pEVar_TextureSRV{};

	static bool SpriteSortByTexture(const VertexSprite& v0, const VertexSprite& v1);
	static bool SpriteSortByDepth(const VertexSprite& v0, const VertexSprite& v1);
};
