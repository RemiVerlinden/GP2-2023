#include "stdafx.h"
#include "PortalGun_Glow_Material.h"
#include "SceneInputDefines\PortalInput.h"

PortalGun_Glow_Material::PortalGun_Glow_Material() :
	Material(L"Effects/Portal/PortalGun_Glow_Material.fx")
{
}

void PortalGun_Glow_Material::SetColor(const XMFLOAT3& color) const
{
	SetVariable_Vector(L"gColor", color);
	m_pPortalGlowColor = PortalScene::Portal::None;
}


void PortalGun_Glow_Material::InitializeEffectVariables()
{
}

void PortalGun_Glow_Material::OnUpdateModelVariables(const SceneContext& context, const ModelComponent* pModel) const
{
	UpdatePlasmaColor(context);

	SetVariable_Scalar(L"gTime", context.pGameTime->GetTotal());

	//Retrieve The Animator from the ModelComponent
	ModelAnimator* pAnimator = pModel->GetAnimator();

	//Make sure the animator is not NULL (ASSERT_NULL_)
	ASSERT_NULL_(pAnimator);

	//Retrieve the BoneTransforms from the Animator
	const std::vector<XMFLOAT4X4>& boneTransforms = pAnimator->GetBoneTransforms();

	//Set the 'gBones' variable of the effect (MatrixArray) > BoneTransforms
	SetVariable_MatrixArray(L"gBones", (float*)boneTransforms.data(), (UINT)boneTransforms.size());
}

void PortalGun_Glow_Material::UpdatePlasmaColor(const SceneContext& /*context*/) const
{
	if (m_pPortalGlowColor != PortalScene::Portal::None)
	{
		if (m_pPortalGlowColor == PortalScene::Portal::Blue)
			SetColor(m_BlueColor);
		else
			SetColor(m_OrangeColor);
	}
}
