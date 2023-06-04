
	#include "stdafx.h"
	#include "PostColorGrading.h"

	PostColorGrading::PostColorGrading():
		PostProcessingMaterial(L"Effects/Post/ColorGrading.fx")
	{
	}

	void PostColorGrading::Initialize(const GameContext&)
	{
		m_pLUT = ContentManager::Load<TextureData>(L"Textures/LUT/portal-LUT.CUBE");

		m_pBaseEffect->GetVariableByName("gColorCorrectionLUT")->AsShaderResource()->SetResource(m_pLUT->GetShaderResourceView());
	}


