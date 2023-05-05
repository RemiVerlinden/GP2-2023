#include "stdafx.h"
#include "PortalManager.h"
//#include "Materials/ColorMaterial.h"
//
//PortalManager::PortalManager(GameScene* pScene)
//	:m_pScene(pScene)
//{
//	InitializePortal(m_pPortals[PortalVariant::Orange]);
//	InitializePortal(m_pPortals[PortalVariant::Blue]);
//}
//
//PortalManager::~PortalManager()
//{
//	m_Lights.clear();
//}
//
//void PortalManager::SetDirectionalLight(const XMFLOAT3& position, const XMFLOAT3& direction)
//{
//	m_DirectionalLight.direction = { direction.x, direction.y, direction.z , 1.0f };
//	m_DirectionalLight.position = { position.x, position.y, position.z , 1.0f };
//}
//
//void PortalManager::InitializePortal(Portal portal)
//{
//	portal.pGameObject = m_pScene->AddChild(new GameObject());
//	portal.pCamera = portal.pGameObject->AddComponent(new CameraComponent());
//
//	ModelComponent* pPortalMesh = portal.pGameObject->AddComponent(new ModelComponent(L"Blender/Portal.ovm"));
//	pPortalMesh->SetMaterial(MaterialManager::Get()->CreateMaterial<ColorMaterial>());
//}
//
//void PortalManager::Update(TransformComponent* characterTransform)
//{
//	// Update the portal camera's position and rotation
//	
//}
//
//// the "Variant" is just another word for the portal number, PortalVariant::orange = 0, PortalVariant::blue = 1
//void PortalManager::MovePortal(PortalVariant variant, TransformComponent* newTransform)
//{
//	if (variant >= PortalVariantCount || variant < 0)
//	{
//		Logger::LogError(L"{} is not a valid Portal ID", (int)variant);
//		assert(false);
//	}
//
//
//	m_pPortals[variant].pGameObject->GetTransform()->Translate(newTransform->GetPosition());
//	XMVECTOR newRotation = XMLoadFloat4(&newTransform->GetRotation());
//	m_pPortals[variant].pGameObject->GetTransform()->Rotate(newRotation);
//}