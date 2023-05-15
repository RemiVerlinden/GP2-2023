#include "stdafx.h"
#include "CameraViewMapRenderer.h"
#include "Misc/CameraViewMapMaterial.h"

CameraViewMapRenderer::~CameraViewMapRenderer()
{
	SafeDelete(m_pCameraRenderTarget)
}

void CameraViewMapRenderer::Initialize()
{
	TODO_W8(L"Implement Initialize")
		//1. Create a separate RenderTarget (see RenderTarget class), store in m_pShadowRenderTarget
		//	- RenderTargets are initialized with the RenderTarget::Create function, requiring a RENDERTARGET_DESC
		//	- Initialize the relevant fields of the RENDERTARGET_DESC (enableColorBuffer:false, enableDepthSRV:true, width & height)

		m_pCameraRenderTarget = new RenderTarget(m_GameContext.d3dContext);

	RENDERTARGET_DESC desc;

	desc.enableColorBuffer = true;
	desc.enableColorSRV = true;
	desc.enableDepthBuffer = true;
	desc.enableDepthSRV = true;
	desc.generateMipMaps_Color = false;
	desc.width = m_GameContext.windowWidth;
	desc.height = m_GameContext.windowHeight;
	desc.depthFormat = DXGI_FORMAT_D32_FLOAT;
	// Add a color format
	desc.colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	HANDLE_ERROR(m_pCameraRenderTarget->Create(desc));

	//2. Create a new ShadowMapMaterial, this will be the material that 'generated' the ShadowMap, store in m_pShadowMapGenerator
	//	- The effect has two techniques, one for static meshes, and another for skinned meshes (both very similar, the skinned version also transforms the vertices based on a given set of boneTransforms)
	//	- We want to store the TechniqueContext (struct that contains information about the Technique & InputLayout required for rendering) for both techniques in the m_GeneratorTechniqueContexts array.
	//	- Use the ShadowGeneratorType enum to retrieve the correct TechniqueContext by ID, and also use that ID to store it inside the array (see BaseMaterial::GetTechniqueContext)

	m_pCameraViewMapGenerator = MaterialManager::Get()->CreateMaterial<CameraViewMapMaterial>();

	m_GeneratorTechniqueContext = m_pCameraViewMapGenerator->GetTechniqueContext();

	m_IsInitialized = true;

	m_InstanceID = m_InstanceCounter;
	++m_InstanceCounter;
}

//void CameraViewMapRenderer::UpdateMeshFilter(const SceneContext& sceneContext, MeshFilter* pMeshFilter) const
//{
//	TODO_W8(L"Implement UpdateMeshFilter")
//		//Here we want to Update the MeshFilter of ModelComponents that need to be rendered to the ShadowMap
//		//Updating the MeshFilter means that we want to create a corresponding VertexBuffer for our ShadowGenerator material
//
//	//2. Retrieve the corresponding TechniqueContext from m_GeneratorTechniqueContexts array (Static/Skinned)
//	const MaterialTechniqueContext& context = m_GeneratorTechniqueContext;
//
//	//3. Build a corresponding VertexBuffer with data from the relevant TechniqueContext you retrieved in Step2 (MeshFilter::BuildVertexBuffer)
//	pMeshFilter->BuildVertexBuffer(sceneContext, context.inputLayoutID, context.inputLayoutSize, context.pInputLayoutDescriptions);
//}

void CameraViewMapRenderer::Begin(const SceneContext& sceneContext, CameraComponent* pCamera)
{
	//m_pCurrentScene = pCamera->GetGameObject()->GetScene();
	//m_pCameraContainer = m_pCurrentScene->GetActiveCamera();
	//m_pCurrentScene->SetActiveCamera(pCamera);

	//sceneContext.pCamera = 
	TODO_W8(L"Implement Begin")
		//This function is called once right before we start the Shadow Pass (= generating the ShadowMap)
		//This function is responsible for setting the pipeline into the correct state, meaning
		//	- Making sure the ShadowMap is unbound from the pipeline as a ShaderResource (SRV), so we can bind it as a RenderTarget (RTV)
		//	- Calculating the Light ViewProjection, and updating the relevant Shader variables
		//	- Binding the ShadowMap RenderTarget as Main Game RenderTarget (= Everything we render is rendered to this rendertarget)
		//	- Clear the current (which should be the ShadowMap RT) rendertarget

		//1. Making sure that the ShadowMap is unbound from the pipeline as ShaderResourceView (SRV) is important, because we cannot use the same resource as a ShaderResourceView (texture resource inside a shader) and a RenderTargetView (target everything is rendered too) at the same time. In case this happens, you'll see an error in the output of visual studio - warning you that a resource is still bound as a SRV and cannot be used as an RTV.
		//	-> Unbinding an SRV can be achieved using DeviceContext::PSSetShaderResource [I'll give you the implementation for free] - double check your output because depending on your usage of ShaderResources, the actual slot the ShadowMap is using can be different, but you'll see a warning pop-up with the correct slot ID in that case.
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(0 , 1, pSRV);


	//3. Update this matrix (m_LightVP) on the ShadowMapMaterial effect
	m_pCameraViewMapGenerator->SetVariable_Matrix(L"gCameraViewProj", pCamera->GetViewProjection());

	//4. Set the Main Game RenderTarget to m_pShadowRenderTarget (OverlordGame::SetRenderTarget) - Hint: every Singleton object has access to the GameContext...
	m_GameContext.pGame->SetRenderTarget(m_pCameraRenderTarget);
	//5. Clear the ShadowMap rendertarget (RenderTarget::Clear)
	m_pCameraRenderTarget->Clear();
}

void CameraViewMapRenderer::DrawMesh(const SceneContext& sceneContext, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>&)
{
	TODO_W8(L"Implement DrawMesh")
		//This function is called for every mesh that needs to be rendered on the shadowmap (= cast shadows)

	//2. Retrieve the correct TechniqueContext for m_GeneratorTechniqueContexts
	//const MaterialTechniqueContext& context = m_GeneratorTechniqueContexts[(int)meshType];

	//3. Set the relevant variables on the ShadowMapMaterial
	//		- world of the mesh
	//		- if animated, the boneTransforms
		m_pCameraViewMapGenerator->SetVariable_Matrix(L"gWorld", meshWorld);

	//4. Setup Pipeline for Drawing (Similar to ModelComponent::Draw, but for our ShadowMapMaterial)
	//	- Set InputLayout (see TechniqueContext)
	//	- Set PrimitiveTopology
	//	- Iterate the SubMeshes of the MeshFilter (see ModelComponent::Draw), for each SubMesh:
	//		- Set VertexBuffer
	//		- Set IndexBuffer
	//		- Set correct TechniqueContext on ShadowMapMaterial - use ShadowGeneratorType as ID (BaseMaterial::SetTechnique)
	//		- Perform Draw Call (same as usual, iterate Technique Passes, Apply, Draw - See ModelComponent::Draw for reference)
	m_pCameraViewMapGenerator->SetTechnique(0);

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(m_pCameraViewMapGenerator->GetTechniqueContext().pInputLayout);

	//Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const auto& subMesh : pMeshFilter->GetMeshes())
	{
		//Set Vertex Buffer
		const UINT offset = 0;
		const auto& vertexBufferData = pMeshFilter->GetVertexBufferData(sceneContext, m_pCameraViewMapGenerator, subMesh.id);
		pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride,
			&offset);

		pDeviceContext->IASetIndexBuffer(subMesh.buffers.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//DRAW
		auto tech = m_pCameraViewMapGenerator->GetTechniqueContext().pTechnique;
		D3DX11_TECHNIQUE_DESC techDesc{};

		tech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(subMesh.indexCount, 0, 0);
		}
	}
}

void CameraViewMapRenderer::End(const SceneContext&) const
{
	TODO_W8(L"Implement End")
		//This function is called at the end of the Shadow-pass, all shadow-casting meshes should be drawn to the ShadowMap at this point.
		//Now we can reset the Main Game Rendertarget back to the original RenderTarget, this also Unbinds the ShadowMapRenderTarget as RTV from the Pipeline, and can safely use it as a ShaderResourceView after this point.

		//1. Reset the Main Game RenderTarget back to default (OverlordGame::SetRenderTarget)
		//		- Have a look inside the function, there is a easy way to do this...
		//m_pCurrentScene->SetActiveCamera(m_pCameraContainer);

		m_GameContext.pGame->SetRenderTarget(nullptr);

}

ID3D11ShaderResourceView* CameraViewMapRenderer::GetColorMap() const
{
	return m_pCameraRenderTarget->GetColorShaderResourceView();
}

void CameraViewMapRenderer::Debug_DrawDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	if (m_pCameraRenderTarget->HasDepthSRV())
	{
		SpriteRenderer::Get()->DrawImmediate(m_GameContext.d3dContext, m_pCameraRenderTarget->GetDepthShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}
