// PortalComponent.h
#pragma once
#include "BaseComponent.h"
//#include "PortalTraveller.h"
//#include "CameraUtility.h"
#include <vector>
#include <memory>
#include <algorithm>
class GameObject;

class PortalComponent final: public BaseComponent
{
public:
	PortalComponent(CameraComponent* playerCamera, bool orangePortal);
	virtual ~PortalComponent();

	PortalComponent(const PortalComponent& other) = delete;
	PortalComponent(PortalComponent&& other) noexcept = delete;
	PortalComponent& operator=(const PortalComponent& other) = delete;
	PortalComponent& operator=(PortalComponent&& other) noexcept = delete;

	void SetLinkedPortal(PortalComponent* pPortal);
	void PrePortalRender(const SceneContext& context);
	void Render(const SceneContext& context);
	void PostPortalRender(const SceneContext& context);
	CameraComponent* GetPortalCamera() const { return m_pPortalCam; }
	PortalComponent* m_pLinkedPortal;
protected:

	void Initialize(const SceneContext& sceneContext) override;
	void PreDraw(const SceneContext&) override;
	void Update(const SceneContext& sceneContext) override;
	//void OnTriggerEnter(Collider* other);
	//void OnTriggerExit(Collider* other);

private:
	virtual void PortalMapDraw(const SceneContext& context);

	//void HandleTravellers();
	//void CreateViewTexture();
	//float ProtectScreenFromClipping(const XMFLOAT3& viewPoint);
	//void UpdateSliceParams(PortalTraveller& traveller);
	//void SetNearClipPlane();
	//void OnTravellerEnterPortal(PortalTraveller* traveller);
	//void HandleClipping();

	//int SideOfPortal(const XMFLOAT3& pos);
	//bool SameSideOfPortal(const XMFLOAT3& posA, const XMFLOAT3& posB);
	//XMFLOAT3 PortalCamPos();

	CameraComponent* m_pPlayerCam;
	GameObject* m_pPortalCameraHolder;
	GameObject* m_pPortalCameraRotator;
	CameraComponent* m_pPortalCam;
	bool bluePortal;

	//std::shared_ptr<RenderTexture> m_pViewTexture;
	//std::vector<std::shared_ptr<PortalTraveller>> m_TrackedTravellers;
};
