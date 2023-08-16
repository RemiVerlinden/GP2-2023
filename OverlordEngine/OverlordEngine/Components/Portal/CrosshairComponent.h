#pragma once
#include "../OverlordProject/Scenes/Portal/PortalScene.h"


class GameObject;
class SpriteFont;
class CrosshairComponent final : public BaseComponent
{
public:
	CrosshairComponent() = default;
	~CrosshairComponent() = default;

	void SetLastPortalFired(PortalScene::Portal portal);

private:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext&) override;
	void Draw(const SceneContext&) override;

	enum class TextureType
	{
		Crosshair,
		EmptyLeft,
		EmptyRight,
		FullLeft,
		FullRight,
		PortalIcon,
		Total
	};
	std::unordered_map<TextureType, SpriteComponent*> m_pTextureComponents;

	XMFLOAT4 m_Blue{ 0.27f,0.553f,0.827f,1.f };
	XMFLOAT4 m_Orange{ 0.81f, 0.56f, 0.31f,1.f };
};