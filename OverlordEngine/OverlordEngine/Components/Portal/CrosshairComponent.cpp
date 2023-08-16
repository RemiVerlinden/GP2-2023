#include "stdafx.h"
#include "CrosshairComponent.h"

void CrosshairComponent::SetLastPortalFired(PortalScene::Portal portal)
{
	XMFLOAT2 pivot = { 500.f,0.5f };
	
	if(portal != PortalScene::Portal::None)
		pivot.x = portal == PortalScene::Portal::Blue ? 1.1f : -0.1f;

	m_pTextureComponents[TextureType::PortalIcon]->SetPivot(pivot);

	m_pTextureComponents[TextureType::PortalIcon]->SetColor(portal == PortalScene::Portal::Blue ? m_Blue : m_Orange);
}

void CrosshairComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	m_Blue.w = 0.4f;
	m_Orange.w = 0.4f;

	std::vector<std::wstring> spriteNames;
	spriteNames.emplace_back(L"crosshair");
	spriteNames.emplace_back(L"emptyleft");
	spriteNames.emplace_back(L"emptyright");
	spriteNames.emplace_back(L"fullleft");
	spriteNames.emplace_back(L"fullright");
	spriteNames.emplace_back(L"portalicon");

	std::vector<XMFLOAT4> spriteColors;
	spriteColors.emplace_back(XMFLOAT4(Colors::White));
	spriteColors.emplace_back(m_Blue);
	spriteColors.emplace_back(m_Orange);
	spriteColors.emplace_back(m_Blue);
	spriteColors.emplace_back(m_Orange);
	spriteColors.emplace_back(XMFLOAT4(m_Orange));

	for (size_t spriteIndex = 0; spriteIndex < spriteNames.size(); ++spriteIndex)
	{
		std::wstring assetpath = std::format(L"Textures/Crosshair/{}.tga", spriteNames[spriteIndex]);

		TextureType type = static_cast<TextureType>(spriteIndex);

		SpriteComponent* spriteComponent = new SpriteComponent(assetpath, DirectX::XMFLOAT2(0.499f, 0.499f), spriteColors[spriteIndex]);
		m_pTextureComponents[type] = m_pGameObject->AddComponent(spriteComponent);
	}

	SetLastPortalFired(PortalScene::Portal::None);

	m_pTextureComponents[TextureType::EmptyLeft]->SetColor({0,0,0,0});
	m_pTextureComponents[TextureType::EmptyRight]->SetColor({0,0,0,0});
}

void CrosshairComponent::Update(const SceneContext& context)
{
	static bool isRunning = true;
	if (context.pGameTime->IsRunning() != isRunning) 
	{
		isRunning = !isRunning;

		for (const auto&[type,component] : m_pTextureComponents)
		{
			component->EnableRender(isRunning);
		}
	}
}

void CrosshairComponent::Draw(const SceneContext& /*context*/)
{

}