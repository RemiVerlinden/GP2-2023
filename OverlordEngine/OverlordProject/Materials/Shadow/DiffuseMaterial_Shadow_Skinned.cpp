#include "stdafx.h"
#include "DiffuseMaterial_Shadow_Skinned.h"

DiffuseMaterial_Shadow_Skinned::DiffuseMaterial_Shadow_Skinned():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow_Skinned.fx")
{}

void DiffuseMaterial_Shadow_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	//  1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	//  LightWVP = model_world * light_viewprojection
	//  (light_viewprojection[LightVP] can be acquired from the ShadowMapRenderer)

	//  2. Update the ShadowMap texture
	auto& cubeMaps = ShadowMapRendererCube::Get()->GetAllShadowCubemaps();
	SetVariable_TextureArray(L"gShadowCubeMap", cubeMaps.data(), (UINT)cubeMaps.size());

	// 3. Update the Light positions (retrieve the positions from the LightManager > sceneContext)
	const auto& lightsVec = sceneContext.pLights->GetLights();
	std::vector<XMFLOAT4> lightPositions;
	for (const Light& light : lightsVec)
		lightPositions.emplace_back(light.position);

	SetVariable_VectorArray(L"gLightPosition", reinterpret_cast<const float*>(lightPositions.data()), (UINT)lightPositions.size());
	
	SetVariable_Scalar(L"gAmountLights", (int)lightsVec.size());

	SetVariable_Scalar(L"gNearPlane", ShadowMapRendererCube::GetNearPlane());
	SetVariable_Scalar(L"gFarPlane", ShadowMapRendererCube::GetFarPlane());

	// 4. Update Bones
	const std::vector<XMFLOAT4X4>& modelBoneTransforms = pModel->GetAnimator()->GetBoneTransforms();
	SetVariable_MatrixArray(L"gBones", reinterpret_cast<const float*>(modelBoneTransforms.data()), static_cast<UINT>(modelBoneTransforms.size()));
}
