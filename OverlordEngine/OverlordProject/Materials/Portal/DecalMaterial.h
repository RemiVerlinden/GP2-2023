#pragma once
class DecalMaterial : public Material<DecalMaterial>
{
public:
	DecalMaterial();
	~DecalMaterial() override = default;

	DecalMaterial(const DecalMaterial& other) = delete;
	DecalMaterial(DecalMaterial&& other) noexcept = delete;
	DecalMaterial& operator=(const DecalMaterial& other) = delete;
	DecalMaterial& operator=(DecalMaterial&& other) noexcept = delete;

	void SetDecalTexture(const std::wstring& assetFile);
protected:
	void InitializeEffectVariables() override;

private:
	TextureData* m_pDecalTexture{};
};