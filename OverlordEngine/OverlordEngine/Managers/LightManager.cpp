#include "stdafx.h"
#include "LightManager.h"
#include "Graphics\ShadowMapRendererCube.h"
LightManager::LightManager()
{
	m_DirectionalLight.direction = { 0.577f, -0.577f, 0.577f , 1.0f };
	m_DirectionalLight.color = { 0.9f, 0.9f, 0.8f, 1.0f };
}

LightManager::~LightManager()
{
	m_Lights.clear();
}

void LightManager::SetDirectionalLight(const XMFLOAT3& position, const XMFLOAT3& direction)
{
	m_DirectionalLight.direction = { direction.x, direction.y, direction.z , 1.0f };
	m_DirectionalLight.position = { position.x, position.y, position.z , 1.0f };
}

UINT LightManager::AddLight(GameScene* pScene, const Light& l)
{
	ShadowMapRendererCube::Get()->AddShadowMap(pScene, l.nearPlane, l.farPlane);

	m_Lights.push_back(l);
	return static_cast<UINT>(m_Lights.size() - 1); 
}
