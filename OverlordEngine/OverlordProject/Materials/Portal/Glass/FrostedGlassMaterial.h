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

	void SetDecalTexture(const std::wstring& assetFile);
protected:
	void InitializeEffectVariables() override;

private:
	TextureData* m_pDecalTexture{};
};