#pragma once
class MonitorMaterial final : public Material<MonitorMaterial>
{
public:
	MonitorMaterial();
	~MonitorMaterial() override = default;

	MonitorMaterial(const MonitorMaterial& other) = delete;
	MonitorMaterial(MonitorMaterial&& other) noexcept = delete;
	MonitorMaterial& operator=(const MonitorMaterial& other) = delete;
	MonitorMaterial& operator=(MonitorMaterial&& other) noexcept = delete;

	
protected:
	void InitializeEffectVariables() override;

	void OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModelComponent) const override;
private:
	TextureData* m_pDiffuseTexture{};
	const std::wstring assetFile = L"Textures/Maps/chamber02_static/~NOyTYmKw_props_lab_monitor_lab_monitor_screen_text.dds";

	struct MonitorProperties 
	{
		float scrollSpeed;
		float startScrollOffset;

		bool scrollUp;
	} MonitorProps;
};

