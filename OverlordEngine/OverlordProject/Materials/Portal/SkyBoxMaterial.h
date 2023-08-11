#pragma once
class SkyBoxmaterial : public Material<SkyBoxmaterial>
{
public:
	SkyBoxmaterial();
	~SkyBoxmaterial() override = default;

	SkyBoxmaterial(const SkyBoxmaterial& other) = delete;
	SkyBoxmaterial(SkyBoxmaterial&& other) noexcept = delete;
	SkyBoxmaterial& operator=(const SkyBoxmaterial& other) = delete;
	SkyBoxmaterial& operator=(SkyBoxmaterial&& other) noexcept = delete;

	void SetSkyBoxTexture(const std::wstring& assetFile);
protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const override;

private:
	TextureData* m_pDiffuseTexture{};
};