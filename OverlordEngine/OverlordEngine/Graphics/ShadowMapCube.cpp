#include "stdafx.h"
#include "Misc/ShadowMapMaterial.h"

using ShadowGeneratorType = ShadowMapRendererCube::ShadowGeneratorType;

ShadowMapCube::ShadowMapCube(const GameContext& gamecontext, float cubemapResolution, float nearPlane, float farPlane)
	:m_GameContext{gamecontext}
	,m_NearPlane{nearPlane}
	,m_FarPlane{farPlane}
{
	m_pShadowRenderTarget = new RenderTarget(gamecontext.d3dContext);

	RENDERTARGET_DESC desc;

	desc.isCubemap = true; // CUBE MAP

	desc.enableDepthBuffer = true;
	desc.enableDepthSRV = true;
	desc.enableColorBuffer = true;
	desc.enableColorSRV = true;
	desc.generateMipMaps_Color = false;
	desc.cubemapResolution = (UINT)cubemapResolution;
	desc.width = (UINT)cubemapResolution;
	desc.height = (UINT)cubemapResolution;

	desc.depthFormat = DXGI_FORMAT_D32_FLOAT;

	HANDLE_ERROR(m_pShadowRenderTarget->Create(desc));
}

ShadowMapCube::~ShadowMapCube()
{
	SafeDelete(m_pShadowRenderTarget)
}

void ShadowMapCube::Begin(const SceneContext& sceneContext, int lightNumber)
{
	const std::vector<Light>& lights = sceneContext.pLights->GetLights();
	if ((lights.size() == 0) || (lights.size() - 1 < lightNumber)) return;

	ID3D11ShaderResourceView* nullSRVs[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 6, nullSRVs);


	const float nearZ = ShadowMapRendererCube::Get()->GetNearPlane();
	const float farZ = ShadowMapRendererCube::Get()->GetFarPlane();
	float aspectRatio = 1;

	std::vector<XMMATRIX> view(6);
	
	m_LightNumber = lightNumber;
	const Light& pointLight = lights[m_LightNumber];

	float fovAngle = XM_PIDIV2;
	const XMMATRIX projection = XMMatrixPerspectiveFovLH(fovAngle, aspectRatio, nearZ, farZ);

	// Get the eyePosition and direction from the spotlight
	auto eyePosition = XMLoadFloat4(&pointLight.position);  // Assuming you have a GetSpotLight() function
	auto direction = XMLoadFloat4(&pointLight.direction);

	std::vector<XMVECTOR> upVec =
	{
		XMVectorSet(0, 1, 0, 0), // Positive X
		XMVectorSet(0, 1, 0, 0), // Negative X
		XMVectorSet(0, 0, -1, 0), // Positive Y
		XMVectorSet(0, 0, 1, 0), // Negative Y
		XMVectorSet(0, 1, 0, 0), // Positive Z
		XMVectorSet(0, 1, 0, 0) // Negative Z
	};

	const XMFLOAT3 directions[6] = {
	XMFLOAT3(1.0f, 0.0f, 0.0f),  // Positive X
	XMFLOAT3(-1.0f, 0.0f, 0.0f), // Negative X
	XMFLOAT3(0.0f, 1.0f, 0.0f),  // Positive Y
	XMFLOAT3(0.0f, -1.0f, 0.0f), // Negative Y
	XMFLOAT3(0.0f, 0.0f, 1.0f),  // Positive Z
	XMFLOAT3(0.0f, 0.0f, -1.0f)  // Negative Z
	};

	m_LightVP.resize(6);
	std::vector<XMMATRIX> viewproj(6);
	for (int i = 0; i < 6; i++)
	{
		direction = XMLoadFloat3(&directions[i]);

		view[i] = XMMatrixLookAtLH(eyePosition, eyePosition + direction, upVec[i]);
		viewproj[i] = view[i] * projection;
		XMStoreFloat4x4(&m_LightVP[i], viewproj[i]);
	}

	m_pShadowRenderTarget->Clear();
}

void ShadowMapCube::DrawMesh(const SceneContext& sceneContext, int face, MeshFilter* pMeshFilter, const XMFLOAT4X4& meshWorld, const std::vector<XMFLOAT4X4>& meshBones)
{
	if (m_LightNumber == -1) return; // if -1 then begin() wasnt able to run and the required variables will possiblibaly not be correctly set

	//This function is called for every mesh that needs to be rendered on the shadowmap (= cast shadows)
	m_GameContext.pGame->SetRenderTarget(m_pShadowRenderTarget, face);

	ShadowMapRendererCube& shadowMapRendererCube = *ShadowMapRendererCube::Get();
	ShadowMapMaterial* pShadowMapGenerator = shadowMapRendererCube.GetShadowMapGenerator();

	//1. Figure out the correct ShadowGeneratorType (Static or Skinned)
	ShadowGeneratorType meshType = pMeshFilter->HasAnimations() ? ShadowGeneratorType::Skinned : ShadowGeneratorType::Static;

	pShadowMapGenerator->SetVariable_Vector(L"gLightPosition", sceneContext.pLights->GetLight(m_LightNumber).position);

	//		- if animated, the boneTransforms
	pShadowMapGenerator->SetVariable_Matrix(L"gWorld", meshWorld);
	if (meshType == ShadowGeneratorType::Skinned)
		pShadowMapGenerator->SetVariable_MatrixArray(L"gBones", reinterpret_cast<const float*>(meshBones.data()), static_cast<UINT>(meshBones.size()));

	pShadowMapGenerator->SetVariable_Matrix(L"gLightViewProj", m_LightVP[face]);

	pShadowMapGenerator->SetTechnique((int)meshType);

	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(pShadowMapGenerator->GetTechniqueContext().pInputLayout);

	//Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (const auto& subMesh : pMeshFilter->GetMeshes())
	{
		//Set Vertex Buffer
		const UINT offset = 0;
		const auto& vertexBufferData = pMeshFilter->GetVertexBufferData(sceneContext, pShadowMapGenerator, subMesh.id);
		pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride,
			&offset);

		pDeviceContext->IASetIndexBuffer(subMesh.buffers.pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		//DRAW
		auto tech = pShadowMapGenerator->GetTechniqueContext().pTechnique;
		D3DX11_TECHNIQUE_DESC techDesc{};

		tech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(subMesh.indexCount, 0, 0);
		}
	}
}

void ShadowMapCube::End(const SceneContext&)
{
	m_GameContext.pGame->SetRenderTarget(nullptr);
	m_LightNumber = -1; // make sure game will break if something goes wrong so I can fix during testing
}

ID3D11ShaderResourceView* ShadowMapCube::GetShadowMap() const
{
	return m_pShadowRenderTarget->GetDepthShaderResourceView();
}

void ShadowMapCube::Debug_DrawDepthSRV(const XMFLOAT2& position, const XMFLOAT2& scale, const XMFLOAT2& pivot) const
{
	if (m_pShadowRenderTarget->HasDepthSRV())
	{
		SpriteRendererCube::Get()->DrawImmediate(m_GameContext.d3dContext, m_pShadowRenderTarget->GetDepthShaderResourceView(), position, XMFLOAT4{ Colors::White }, pivot, scale);

		//Remove from Pipeline
		constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
		m_GameContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
}
