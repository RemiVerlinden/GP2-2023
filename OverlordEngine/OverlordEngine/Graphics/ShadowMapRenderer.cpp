#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "Misc/ShadowMapMaterial.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	SafeDelete(m_pShadowRenderTarget)
}

void ShadowMapRenderer::Initialize()
{
	TODO_W8(L"Implement Initialize")
		//1. Create a separate RenderTarget (see RenderTarget class), store in m_pShadowRenderTarget
		//	- RenderTargets are initialized with the RenderTarget::Create function, requiring a RENDERTARGET_DESC
		//	- Initialize the relevant fields of the RENDERTARGET_DESC (enableColorBuffer:false, enableDepthSRV:true, width & height)

		m_pShadowRenderTarget = new RenderTarget(m_GameContext.d3dContext);

	RENDERTARGET_DESC desc;

	desc.enableDepthBuffer = true;
	desc.enableDepthSRV = true;
	desc.enableColorBuffer = false;
	desc.enableColorSRV = false;
	desc.generateMipMaps_Color = false;
	desc.width = m_GameContext.windowWidth;
	desc.height = m_GameContext.windowHeight;
	desc.depthFormat = DXGI_FORMAT_D32_FLOAT;


	HANDLE_ERROR(m_pShadowRenderTarget->Create(desc));

	m_pShadowMapGenerator = MaterialManager::Get()->CreateMaterial<ShadowMapMaterial>();

	for (int ID = 0; ID < (int)ShadowGeneratorType::Count; ++ID)
		m_GeneratorTechniqueContexts[ID] = m_pShadowMapGenerator->GetTechniqueContext(ID);

	m_IsInitialized = true;
}

void ShadowMapRenderer::UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const
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

void ShadowMapRenderer::Begin(const SceneContext& sceneContext)
{
	TODO_W8(L"Implement Begin")

		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	float viewWidth = 100.f * sceneContext.aspectRatio;
	float viewHeight = 100.0f;
	float nearZ = 0.1f;
	float farZ = 500.0f;
	XMMATRIX projection = XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);

	//- Use XMMatrixLookAtLH to create a View Matrix
	//		*eyePosition: Position of the Direction Light (SceneContext::pLights > Retrieve Directional Light)
	//		*focusPosition: Calculate using the Direction Light position and direction
	auto eyePosition = XMLoadFloat4(&sceneContext.pLights->GetDirectionalLight().position);
	auto direction = XMLoadFloat4(&sceneContext.pLights->GetDirectionalLight().direction);
	XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
	auto view = XMMatrixLookAtLH(eyePosition, eyePosition + direction, upVec);

	//float fovAngle = XMConvertToRadians(90.f);  // 45 degrees FOV, adjust this to your spotlight's cone angle.
	//XMMATRIX projection = XMMatrixPerspectiveFovLH(fovAngle, sceneContext.aspectRatio, nearZ, farZ);

	//// Get the eyePosition and direction from the spotlight
	//auto eyePosition = XMLoadFloat4(&sceneContext.pLights->GetLight(0).position);  // Assuming you have a GetSpotLight() function
	//auto direction = XMLoadFloat4(&sceneContext.pLights->GetLight(0).direction);

	//XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
	//auto view = XMMatrixLookAtLH(eyePosition, eyePosition + direction, upVec);



	//- Use the Projection & View Matrix to calculate the ViewProjection of this Light, store in m_LightVP
	XMMATRIX viewproj = view * projection;
	XMStoreFloat4x4(&m_LightVP, viewproj);


	//3. Update this matrix (m_LightVP) on the ShadowMapMaterial effect
	m_pShadowMapGenerator->SetVariable_Matrix(L"gLightViewProj", m_LightVP);

	//4. Set the Main Game RenderTarget to m_pShadowRenderTarget (OverlordGame::SetRenderTarget) - Hint: every Singleton object has access to the GameContext...
	m_GameContext.pGame->SetRenderTarget(m_pShadowRenderTarget);
	//5. Clear the ShadowMap rendertarget (RenderTarget::Clear)
	m_pShadowRenderTarget->Clear();
}

void ShadowMapRenderer::DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones)
{
	TODO_W8(L"Implement DrawMesh")
		//This function is called for every mesh that needs to be rendered on the shadowmap (= cast shadows)

		//1. Figure out the correct ShadowGeneratorType (Static or Skinned)
		ShadowGeneratorType meshType = pMeshFilter->HasAnimations() ? ShadowGeneratorType::Skinned : ShadowGeneratorType::Static;

	
	//		- if animated, the boneTransforms
	m_pShadowMapGenerator->SetVariable_Matrix(L"gWorld", meshWorld);
	if (meshType == ShadowGeneratorType::Skinned)
		m_pShadowMapGenerator->SetVariable_MatrixArray(L"gBones", reinterpret_cast<const float*>(meshBones.data()), static_cast<UINT>(meshBones.size()));


	m_pShadowMapGenerator->SetTechnique((int)meshType);

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(m_pShadowMapGenerator->GetTechniqueContext().pInputLayout);

	//Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const auto& subMesh : pMeshFilter->GetMeshes())
	{
		//Set Vertex Buffer
		const UINT offset = 0;
		const auto& vertexBufferData = pMeshFilter->GetVertexBufferData(sceneContext, m_pShadowMapGenerator, subMesh.id);
		pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride,
			&offset);

		pDeviceContext->IASetIndexBuffer(subMesh.buffers.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//DRAW
		auto tech = m_pShadowMapGenerator->GetTechniqueContext().pTechnique;
		D3DX11_TECHNIQUE_DESC techDesc{};

		tech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(subMesh.indexCount, 0, 0);
		}
	}
}

void ShadowMapRenderer::End(const SceneContext& /*sceneContext*/) const
{
	TODO_W8(L"Implement End")

		m_GameContext.pGame->SetRenderTarget(nullptr);


		//// Create a device context
		//ID3D11DeviceContext* deviceContext = sceneContext.d3dContext.pDeviceContext; // you should actually retrieve your actual device context here

		//// Number of slots that shader resource views can be bound to, usually 128
		//const UINT numSRVSlots = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;

		//// Array of ID3D11ShaderResourceView pointers, initialized to nullptr
		//ID3D11ShaderResourceView* shaderResourceViews[numSRVSlots] = {};

		//// Get shader resource views
		//deviceContext->PSGetShaderResources(0, numSRVSlots, shaderResourceViews);

		//// Print each shader resource view
		//for (UINT i = 0; i < numSRVSlots; ++i)
		//{
		//	if (shaderResourceViews[i] != nullptr)
		//	{
		//		// Do something with shaderResourceViews[i], such as printing its details
		//		// The actual implementation depends on your needs
		//		std::cout << "SRV bound at slot " << i << std::endl;

		//		// Don't forget to release the SRV
		//		shaderResourceViews[i]->Release();
		//	}
		//}
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	return m_pShadowRenderTarget->GetDepthShaderResourceView();
}

void ShadowMapRenderer::Debug_DrawDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	if (m_pShadowRenderTarget->HasDepthSRV())
	{
		SpriteRenderer::Get()->DrawImmediate(m_GameContext.d3dContext, m_pShadowRenderTarget->GetDepthShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}
