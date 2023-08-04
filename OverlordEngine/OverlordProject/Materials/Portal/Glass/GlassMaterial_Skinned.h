#pragma once
class GlassMaterial_Skinned : public Material<GlassMaterial_Skinned>
{
public:
	GlassMaterial_Skinned();
	~GlassMaterial_Skinned() override = default;

	GlassMaterial_Skinned(const GlassMaterial_Skinned& other) = delete;
	GlassMaterial_Skinned(GlassMaterial_Skinned&& other) noexcept = delete;
	GlassMaterial_Skinned& operator=(const GlassMaterial_Skinned& other) = delete;
	GlassMaterial_Skinned& operator=(GlassMaterial_Skinned&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetOpacity(const float opacity);
protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext& /*sceneContext*/, const ModelComponent* /*pModel*/) const;
private:
	TextureData* m_pDiffuseTexture{};
	float m_Opacity{};
};