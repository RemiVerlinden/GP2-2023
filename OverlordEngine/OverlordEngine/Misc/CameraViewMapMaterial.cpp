#include "stdafx.h"
#include "CameraViewMapMaterial.h"

CameraViewMapMaterial::CameraViewMapMaterial():
	Material(L"Effects/Portal/CameraViewMapGenerator.fx")
{
}

void CameraViewMapMaterial::InitializeEffectVariables()
{
}
