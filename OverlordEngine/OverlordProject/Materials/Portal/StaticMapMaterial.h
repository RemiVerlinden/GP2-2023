#pragma once
class StaticMapMaterial : public Material<StaticMapMaterial>
{
public:
	StaticMapMaterial();
	~StaticMapMaterial() override = default;

	StaticMapMaterial(const StaticMapMaterial& other) = delete;
	StaticMapMaterial(StaticMapMaterial&& other) noexcept = delete;
	StaticMapMaterial& operator=(const StaticMapMaterial& other) = delete;
	StaticMapMaterial& operator=(StaticMapMaterial&& other) noexcept = delete;

	void SetDiffuseTexture(const std::wstring& assetFile);
protected:
	void InitializeEffectVariables() override;
	//void OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const override;

private:
	TextureData* m_pDiffuseTexture{};
};