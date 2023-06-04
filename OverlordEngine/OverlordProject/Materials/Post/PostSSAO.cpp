
	#include "stdafx.h"
	#include "PostSSAO.h"

	PostSSAO::PostSSAO():
		PostProcessingMaterial(L"Effects/Post/SSAO.fx")
	{
	}

	void PostSSAO::UpdateBaseEffectVariables(const SceneContext& /*sceneContext*/, RenderTarget* pSource)
	{
		const auto pSourceSRV = pSource->GetDepthShaderResourceView();
		m_pBaseEffect->GetVariableByName("gDepthTexture")->AsShaderResource()->SetResource(pSourceSRV);
	}


