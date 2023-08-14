#pragma once
class PhongMaterial : public Material<PhongMaterial>
{
public:
	PhongMaterial();
	~PhongMaterial() override = default;

	PhongMaterial(const PhongMaterial& other) = delete;
	PhongMaterial(PhongMaterial&& other) noexcept = delete;
	PhongMaterial& operator=(const PhongMaterial& other) = delete;
	PhongMaterial& operator=(PhongMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetNormalTexture(const std::wstring& assetFile);
	void SetSpecularTexture(const std::wstring& assetFile);
	bool UsingSpecularTexture() const { return m_UseSpecularTexture; }
protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const override;

private:
	TextureData* m_pDiffuseTexture{};
	TextureData* m_pNormalTexture{};
	TextureData* m_pSpecularTexture{};

	bool m_UseSpecularTexture = false;
};