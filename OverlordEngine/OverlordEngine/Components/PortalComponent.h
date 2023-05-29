// PortalComponent.h
#pragma once
#include "BaseComponent.h"
//#include "PortalTraveller.h"
//#include "CameraUtility.h"
#include <vector>
#include <memory>
#include <algorithm>
class GameObject;
class Character;
class PortalComponent final: public BaseComponent
{
public:
	PortalComponent(Character* playerCharacter, bool orangePortal);
	virtual ~PortalComponent();

	PortalComponent(const PortalComponent& other) = delete;
	PortalComponent(PortalComponent&& other) noexcept = delete;
	PortalComponent& operator=(const PortalComponent& other) = delete;
	PortalComponent& operator=(PortalComponent&& other) noexcept = delete;

	void SetLinkedPortal(PortalComponent* pPortal);
	CameraComponent* GetPortalCamera() const { return m_pPortalCam; }
	GameObject* GetCameraRotator() const { return m_pPortalCameraRotator; }
protected:

	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;
	void PostUpdate(const SceneContext& sceneContext) override;
	//void OnTriggerEnter(Collider* other);
	//void OnTriggerExit(Collider* other);

private:
	virtual void PortalMapDraw(const SceneContext& context);
	void HandlePortalCameraClipPlane();

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

	PortalComponent* m_pLinkedPortal;
	Character* m_pPlayerCharacter;
	CameraComponent* m_pPlayerCam;
	GameObject* m_pPortalCameraHolder;
	GameObject* m_pPortalCameraRotator;
	CameraComponent* m_pPortalCam;
	bool bluePortal;

	float m_NearClipPlaneOffset = 0.05f;
	float m_NearClipPlaneLimit = 0.2f;

	//std::shared_ptr<RenderTexture> m_pViewTexture;
	//std::vector<std::shared_ptr<PortalTraveller>> m_TrackedTravellers;
};
