#include "stdafx.h"
#include "PortalRenderer.h"
#include "Components\PortalComponent.h"


RenderTarget* PortalRenderer::GetPortalRenderTarget(Portal portalColor)
{
	return m_pPortalRenderTarget.at((size_t)portalColor);
}

void PortalRenderer::Initialize()
{
	m_pPortalRenderTarget.fill(nullptr);
	m_pPortalComponents.fill(nullptr);

	InitializeRenderTarget(Portal::orange);
	InitializeRenderTarget(Portal::blue);

	m_IsInitialized = true;
}

void PortalRenderer::InitializePortalComponents(std::array<PortalComponent*, 2> portalComponents)
{
	if (std::any_of(portalComponents.begin(), portalComponents.end(), [](PortalComponent* portalComponent) { return portalComponent == nullptr; }))
	{
		assert(false && "portals must not be set to nullptr, call InitializePortalComponents() with valid pointers");
		return;
	}

	m_PortalsInitialized = true;
	m_pPortalComponents = portalComponents;
}
// we need the scene pointer to be able to change the active camera
void PortalRenderer::RenderPortals(const SceneContext& sceneContext, GameScene* pScene)
{
	// make sure the m_pPortalComponents are not nullptr
	if (!m_PortalsInitialized)
	{
		assert(false && "the renderer needs acces to the portalComponents, call InitializePortalComponents()");
		return;
	}

	for (size_t currentPortal = 0 ; currentPortal < (size_t)Portal::count; ++currentPortal)
	{
		PortalRenderingContext renderContext
		{
			static_cast<Portal>(currentPortal),
			m_pPortalRenderTarget.at(currentPortal),
			sceneContext,
			pScene,
			sceneContext.pCamera
		};

		Begin(renderContext);
		Render(renderContext);
		End(renderContext);
	}
}

bool PortalRenderer::IsGameObjectPortal(GameObject* gameObject)
{
	// input or portal components arenot valid
	if (gameObject == nullptr || !m_PortalsInitialized)
	{
		assert(false);
		return false;
	}

	bool result = std::any_of(m_pPortalComponents.begin(),m_pPortalComponents.end(),[gameObject](PortalComponent* portalComponent)
		{
			return portalComponent != nullptr && portalComponent->GetGameObject() == gameObject;
		}
	);
	return result;
}



PortalRenderer::~PortalRenderer()
{
	for (RenderTarget* renderTarget : m_pPortalRenderTarget)
	{
		SafeDelete(renderTarget);
	}
}

void PortalRenderer::Begin(PortalRenderingContext& renderContext)
{
	m_CurrentlyRenderingPortals = true;

	UINT portalID = static_cast<UINT>(renderContext.portalColor);
	GameObject *pPortalObject = m_pPortalComponents.at(portalID)->GetGameObject();

	CameraComponent* pPortalCamera = pPortalObject->GetComponent<PortalComponent>()->GetPortalCamera();
	if (pPortalCamera == nullptr)
	{
		assert(false);
	}
	renderContext.pCurrentScene->SetActiveCamera(pPortalCamera);


	m_GameContext.pGame->SetRenderTarget(renderContext.pRenderTarget);
	renderContext.pRenderTarget->Clear();
 }

void PortalRenderer::Render(PortalRenderingContext& renderContext)
{
	renderContext.pCurrentScene->SimpleDrawMeshes();
}

void PortalRenderer::End(PortalRenderingContext& renderContext)
{
	renderContext.pCurrentScene->SetActiveCamera(renderContext.playerCamera);
	m_GameContext.pGame->SetRenderTarget(nullptr);
	m_CurrentlyRenderingPortals = false;

	constexpr ID3D11ShaderResourceView* const pSRV[] = { nullptr };
	renderContext.sceneContext.d3dContext.pDeviceContext->PSSetShaderResources(0, 1, pSRV);


	//// Create a device context
	//ID3D11DeviceContext* deviceContext = renderContext.sceneContext.d3dContext.pDeviceContext; // you should actually retrieve your actual device context here

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

void PortalRenderer::InitializeRenderTarget(Portal type)
{
	RenderTarget* renderTarget = new RenderTarget(m_GameContext.d3dContext);

	RENDERTARGET_DESC desc;

	desc.enableDepthBuffer = true;
	desc.enableDepthSRV = true;
	desc.enableColorBuffer = true;
	desc.enableColorSRV = true;
	desc.generateMipMaps_Color = false;
	desc.width = m_GameContext.windowWidth;
	desc.height = m_GameContext.windowHeight;
	desc.colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;


	HANDLE_ERROR(renderTarget->Create(desc));

	UINT portalID = static_cast<UINT>(type);
	m_pPortalRenderTarget[portalID] = renderTarget;
}
