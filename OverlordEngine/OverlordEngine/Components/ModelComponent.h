#pragma once
#include "BaseComponent.h"

class MeshFilter;
class ModelAnimator;

class ModelComponent : public BaseComponent
{
public:
	ModelComponent(const std::wstring&  assetFile, bool castShadows = true);
	~ModelComponent() override;

	ModelComponent(const ModelComponent& other) = delete;
	ModelComponent(ModelComponent&& other) noexcept = delete;
	ModelComponent& operator=(const ModelComponent& other) = delete;
	ModelComponent& operator=(ModelComponent&& other) noexcept = delete;

	void SetMaterial(BaseMaterial* pMaterial, UINT8 submeshId = 0);
	void SetMaterial(UINT materialId, UINT8 submeshId = 0);

	ModelAnimator* GetAnimator() const;
	bool HasAnimator() const { return m_pAnimator != nullptr; }

	enum class PortalRenderContext
	{
		RealWorldOnly,  // For objects like the player's weapon, etc...
		PortalViewOnly, // For objects that should appear only when viewed through portals, like other side of the portal, player character, etc...
		Everywhere,     // For standard objects that appear both in the real world and in the portal view.
	};

	void SetPortalrRenderContext(PortalRenderContext renderContext) { m_RenderContext = renderContext; }
	void SetCastShadows(bool castShadows) { m_enableShadowMapDraw = castShadows; }
protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext& sceneContext) override;

	void ShadowMapDraw(const SceneContext& sceneContext) override; //update_W9
	void ShadowMapCubeDraw(const SceneContext& sceneContext) override; 

private:
	std::wstring m_AssetFile{};
	MeshFilter* m_pMeshFilter{};

	std::vector<BaseMaterial*> m_Materials{};
	BaseMaterial* m_pDefaultMaterial{};

	bool m_MaterialChanged{};

	ModelAnimator* m_pAnimator{};

	PortalRenderContext m_RenderContext = PortalRenderContext::Everywhere;

	//W9
	bool m_CastShadows{ true };
};
