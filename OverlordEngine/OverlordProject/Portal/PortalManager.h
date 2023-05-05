//#pragma once
//#include <array>
//
//enum PortalVariant
//{
//	Orange,
//	Blue,
//	PortalVariantCount
//};
//
////__declspec(align(16)) struct Light //(with 24B padding)
//struct Light
//{
//	XMFLOAT4 direction; //16B
//	XMFLOAT4 position; //16B
//	XMFLOAT4 color; //16B
//	float intensity; //4B
//	float range; //4B
//	float spotLightAngle; //4B
//	LightType type; //4B
//	//4 * 16B (To Shader)
//
//	bool isEnabled; //4B (Discard)
//
//	Light() :
//		direction({ 0.f, 1.f, 0.f, 1.f }),
//		position({ 0.f, 0.f, 0.f, 0.f }),
//		color({ 1.f, 1.f, 1.f, 1.f }),
//		intensity(1.f),
//		range(50.f),
//		spotLightAngle(45.f),
//		type(LightType::Point),
//		isEnabled(true) {}
//};
//
//struct Portal
//{
//	PortalVariant variant;
//	GameObject* pGameObject;
//	CameraComponent* pCamera;
//};
//
//class PortalManager
//{
//public:
//	PortalManager(GameScene* pScene);
//	~PortalManager();
//	PortalManager(const PortalManager& other) = delete;
//	PortalManager(PortalManager&& other) noexcept = delete;
//	PortalManager& operator=(const PortalManager& other) = delete;
//	PortalManager& operator=(PortalManager&& other) noexcept = delete;
//
//	//For Deferred Rendering
//	UINT AddLight(const Light& l) { m_Lights.push_back(l); return static_cast<UINT>(m_Lights.size() - 1); }
//	Light& GetLight(int id) { return m_Lights[id]; }
//	const std::vector<Light>& GetLights() const { return m_Lights; }
//
//	Light& GetDirectionalLight() { return m_DirectionalLight; }
//	void SetDirectionalLight(const XMFLOAT3& position, const XMFLOAT3& direction);
//
//	void Update(TransformComponent* characterTransform);
//
//	void InitializePortal(Portal portal);
//	void MovePortal(PortalVariant variant, TransformComponent* newTransform);
//private:
//	GameScene* m_pScene{};
//	std::vector<Light> m_Lights{};
//	Light m_DirectionalLight{};
//	std::array<Portal, 2> m_pPortals{};
//
//};
//
