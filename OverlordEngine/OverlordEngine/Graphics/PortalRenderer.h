#pragma once
#include <array>
class PortalComponent;

	class PortalRenderer : public Singleton<PortalRenderer>
	{
	public:
		void InitializePortalComponents(std::array<PortalComponent*, 2> portalComponents);
		bool IsRenderingPortals() const { return m_CurrentlyRenderingPortals; }
		void RenderPortals(const SceneContext& sceneContext, GameScene* pScene);
		bool IsGameObjectPortal(GameObject* gameObject);
		enum class Portal
		{
			blue,
			orange,
			count
		};
		RenderTarget* GetPortalRenderTarget(Portal portalColor);
	protected:
		void Initialize() override;
	private:
		friend class Singleton<PortalRenderer>;
		PortalRenderer() = default;
		~PortalRenderer();


		struct PortalRenderingContext
		{
			Portal portalColor;
			RenderTarget* pRenderTarget;
			const SceneContext& sceneContext;
			GameScene* pCurrentScene;
			CameraComponent* playerCamera;
		};

		void Begin(PortalRenderingContext& renderContext);
		void Render(PortalRenderingContext& renderContext);
		void End(PortalRenderingContext& renderContext);




		void InitializeRenderTarget(Portal type);

		std::array<RenderTarget*,2> m_pPortalRenderTarget;
		std::array<PortalComponent*,2> m_pPortalComponents;
		bool m_PortalsInitialized = false;
		bool m_CurrentlyRenderingPortals = false;
	};
