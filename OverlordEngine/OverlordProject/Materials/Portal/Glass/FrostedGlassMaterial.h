#pragma once
class FrostedGlassMaterial : public Material<FrostedGlassMaterial>
{
public:
	FrostedGlassMaterial();
	~FrostedGlassMaterial() override = default;

	FrostedGlassMaterial(const FrostedGlassMaterial& other) = delete;
	FrostedGlassMaterial(FrostedGlassMaterial&& other) noexcept = delete;
	FrostedGlassMaterial& operator=(const FrostedGlassMaterial& other) = delete;
	FrostedGlassMaterial& operator=(FrostedGlassMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetOpacity(const float opacity);
protected:
	void InitializeEffectVariables() override;

private:
	TextureData* m_pDiffuseTexture{};
	float m_Opacity{};
};