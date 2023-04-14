#include "stdafx.h"
#include "SpikyMaterial.h"

SpikyMaterial::SpikyMaterial()
	:Material<SpikyMaterial>(L"Labs/Week4/Effects/SpikyShader.fx")
{
}

void SpikyMaterial::InitializeEffectVariables()
{
}
