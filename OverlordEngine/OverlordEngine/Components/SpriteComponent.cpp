#include "stdafx.h"
#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(const std::wstring& spriteAsset, const XMFLOAT2& pivot, const XMFLOAT4& color):
	m_SpriteAsset(spriteAsset),
	m_Pivot(pivot),
	m_Color(color)
{}

void SpriteComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::Draw(const SceneContext& sceneContext)
{
	if (!m_pTexture)
		return;

	TODO_W4(L"Draw the sprite with SpriteRenderer::Draw")

	//Here you need to draw the SpriteComponent using the Draw of the sprite renderer
	// The sprite renderer is a singleton
	// you will need to position (X&Y should be in screenspace, Z contains the depth between [0,1]), the rotation and the scale from the owning GameObject
	// You can use the MathHelper::QuaternionToEuler function to help you with the z rotation 

		TransformComponent* transform
	{
		m_pGameObject->GetTransform()
	};

	SpriteRenderer::Get()->AppendSprite(m_pTexture,
		DirectX::XMFLOAT2{ transform->GetWorldPosition().x, transform->GetWorldPosition().y },
		m_Color,
		m_Pivot,
		DirectX::XMFLOAT2{ transform->GetScale().x, transform->GetScale().y },
		MathHelper::QuaternionToEuler(transform->GetWorldRotation()).z,
		transform->GetPosition().z
	);

	SpriteRenderer::Get()->Draw(sceneContext);
}