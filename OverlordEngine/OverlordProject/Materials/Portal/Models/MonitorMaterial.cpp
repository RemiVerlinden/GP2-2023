#include "stdafx.h"
#include "MonitorMaterial.h"

MonitorMaterial::MonitorMaterial()
	:Material<MonitorMaterial>(L"Effects/Portal/MonitorMaterial.fx")
{
}

void MonitorMaterial::InitializeEffectVariables()
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture->GetShaderResourceView());

	MonitorProps.scrollSpeed = (rand() % 100) / 200 + 0.1f;
	MonitorProps.startScrollOffset = float(rand() % 600);  // random number between 0 and 599 (seconds, so up to 10 minutes)

	MonitorProps.scrollUp = rand() % 2 == 0;
}

void MonitorMaterial::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* /*pModelComponent*/) const
{
	float time = MonitorProps.startScrollOffset;

	time +=	sceneContext.pGameTime->GetTotal(); // add current time
	time *= MonitorProps.scrollUp ? 1 : -1;

	SetVariable_Scalar(L"gTime", time);
	SetVariable_Scalar(L"gScrollSpeed", MonitorProps.scrollSpeed);
}

