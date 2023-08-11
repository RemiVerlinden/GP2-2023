#pragma once
class ShadowMapMaterialCube final: public Material<ShadowMapMaterialCube>
{
public:
	ShadowMapMaterialCube();
	~ShadowMapMaterialCube() override = default;
	ShadowMapMaterialCube(const ShadowMapMaterialCube& other) = delete;
	ShadowMapMaterialCube(ShadowMapMaterialCube&& other) noexcept = delete;
	ShadowMapMaterialCube& operator=(const ShadowMapMaterialCube& other) = delete;
	ShadowMapMaterialCube& operator=(ShadowMapMaterialCube&& other) noexcept = delete;

protected:
	void InitializeEffectVariables() override;
};

