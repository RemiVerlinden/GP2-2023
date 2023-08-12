#pragma once

struct RENDERTARGET_DESC
{
	bool isCubemap{ false };
	// prefer a value that is the power of 2 (ex: 512 or 1024 )
	UINT cubemapResolution{ 0 }; // cubemap must have square texture so this counts as width and height

	UINT width{0};
	UINT height{0};

	//Enable this if you want to create a DepthStencilBuffer
	bool enableDepthBuffer{true};
	//Enable this if you want to use the DepthStencilBuffer
	//as a ShaderResourceView (Texture in a shader)
	bool enableDepthSRV{false};
	//Enable this if you want to create a RenderTarget (color)
	bool enableColorBuffer{true};
	//Enable this if you want to use the RenderTarget
	//as a ShaderResourceView (Texture in a shader)
	bool enableColorSRV{false};
	//Generate MipMaps (ColorSRV Only)
	bool generateMipMaps_Color;

	//Depth buffer format (DepthStencil)
	DXGI_FORMAT depthFormat{ DXGI_FORMAT_D32_FLOAT };
	//Color buffer format (Rendertarget)
	DXGI_FORMAT colorFormat{ DXGI_FORMAT_R8G8B8A8_UNORM };

	//Optional: Supply one of the buffers
	ID3D11Texture2D* pColor{nullptr};
	ID3D11Texture2D* pDepth{nullptr};

	void IsValid()
	{
		ASSERT_IF(!(pColor || pDepth) && (width <= 0 || height <= 0), L"Invalid Width and/or Height for RenderTarget");
		ASSERT_IF(!(enableColorBuffer || enableDepthBuffer), L"Rendertarget must contain at least one buffer! (Color AND/OR Depth)");

		if (enableDepthSRV && !enableDepthBuffer)
		{
			enableDepthBuffer = true;
			Logger::LogWarning(L"Forced \'enableDepthBuffer\' flag because \'enableDepthSRV\' is enabled by user.");
		}

		if (enableColorSRV && !enableColorBuffer)
		{
			enableColorBuffer = true;
			Logger::LogWarning(L"Forced \'enableColorBuffer\' flag because \'enableColorSRV\' is enabled by user.");
		}
	}
};

class RenderTarget
{
public:
	RenderTarget(const D3D11Context& d3dContext);
	~RenderTarget();

	RenderTarget(const RenderTarget& other) = delete;
	RenderTarget(RenderTarget&& other) noexcept = delete;
	RenderTarget& operator=(const RenderTarget& other) = delete;
	RenderTarget& operator=(RenderTarget&& other) noexcept = delete;

	HRESULT Create(RENDERTARGET_DESC desc);
	void Clear(XMFLOAT4 clearColor = XMFLOAT4{Colors::CornflowerBlue}) const;

	ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }

	ID3D11RenderTargetView* GetRenderTargetViewForFace(UINT face) const;
	ID3D11DepthStencilView* GetDepthStencilViewForFace(UINT face) const;

	ID3D11ShaderResourceView* GetColorShaderResourceView() const;
	ID3D11ShaderResourceView* GetDepthShaderResourceView() const;

	bool HasRenderTargetView() const { return m_pRenderTargetView != nullptr; }
	bool HasDepthDepthStencilView() const { return m_pDepthStencilView != nullptr; }

	bool HasColorSRV() const { return m_Desc.enableColorSRV; }
	bool HasDepthSRV() const { return m_Desc.enableDepthSRV; }

	const RENDERTARGET_DESC& GetDesc() const { return m_Desc; }

private:

	D3D11Context m_D3DContext{};
	
	RENDERTARGET_DESC m_Desc{};
	ID3D11RenderTargetView* m_pRenderTargetView{nullptr};
	ID3D11ShaderResourceView* m_pColorShaderResourceView{nullptr};
	ID3D11ShaderResourceView* m_pDepthShaderResourceView{ nullptr };
	ID3D11Texture2D* m_pColor{ nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView{ nullptr };
	ID3D11Texture2D* m_pDepth{ nullptr };
	std::vector<ID3D11RenderTargetView*> m_pRTVcube;
	std::vector<ID3D11DepthStencilView*> m_pDSVcube;



	HRESULT CreateColor();
	HRESULT CreateDepth();

	static DXGI_FORMAT GetResourceFormat_Depth(DXGI_FORMAT sourceFormat);
	static DXGI_FORMAT GetShaderResourceViewFormat_Depth(DXGI_FORMAT sourceFormat);

	void CleanUp();

	bool m_ColorBufferSupplied{false};
	bool m_DepthBufferSupplied{false};
};

