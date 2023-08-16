#pragma once
#include "../OverlordProject/Scenes/Portal/PortalScene.h"
class PortalGun_Glow_Material : public Material<PortalGun_Glow_Material>
{
public:
	PortalGun_Glow_Material();
	~PortalGun_Glow_Material() override = default;

	PortalGun_Glow_Material(const PortalGun_Glow_Material& other) = delete;
	PortalGun_Glow_Material(PortalGun_Glow_Material&& other) noexcept = delete;
	PortalGun_Glow_Material& operator=(const PortalGun_Glow_Material& other) = delete;
	PortalGun_Glow_Material& operator=(PortalGun_Glow_Material&& other) noexcept = delete;

	void SetColor(const XMFLOAT3& color) const;

	static void SetGlowColor(PortalScene::Portal color) { m_pPortalGlowColor = color; };
protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext& /*sceneContext*/, const ModelComponent* /*pModel*/) const;
private:
	void UpdatePlasmaColor(const SceneContext& context) const;
	XMFLOAT3 m_BlueColor {0.f, 0.6f, 1.f};
	XMFLOAT3 m_OrangeColor{1.f,0.6f,0.f};

	// im kinda using this right now because I dont have a eventlistener or observer pattern yet
	inline static PortalScene::Portal m_pPortalGlowColor = PortalScene::Portal::None;
};