#pragma once
class PhongMaterial_Skinned : public Material<PhongMaterial_Skinned>
{
public:
	PhongMaterial_Skinned();
	~PhongMaterial_Skinned() override = default;

	PhongMaterial_Skinned(const PhongMaterial_Skinned& other) = delete;
	PhongMaterial_Skinned(PhongMaterial_Skinned&& other) noexcept = delete;
	PhongMaterial_Skinned& operator=(const PhongMaterial_Skinned& other) = delete;
	PhongMaterial_Skinned& operator=(PhongMaterial_Skinned&& other) noexcept = delete;

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