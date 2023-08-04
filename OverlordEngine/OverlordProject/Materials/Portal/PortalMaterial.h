#pragma once
class PortalComponent;
class PortalMaterial : public Material<PortalMaterial>
{
public:
	PortalMaterial(); 
	~PortalMaterial() override = default;

	PortalMaterial(const PortalMaterial& other) = delete;
	PortalMaterial(PortalMaterial&& other) noexcept = delete;
	PortalMaterial& operator=(const PortalMaterial& other) = delete;
	PortalMaterial& operator=(PortalMaterial&& other) noexcept = delete;

    void SetPortalComponent(PortalComponent* pComponent);

protected:
	void InitializeEffectVariables() override;
	void OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const override;

	PortalRenderer::Portal m_Portal{};
private:
	TextureData* m_pDiffuseTexture{};
	PortalComponent* m_pPortalComponent{};
};


class OrangePortalMaterial final : public PortalMaterial
{
public:
    OrangePortalMaterial();
    ~OrangePortalMaterial() override = default;

    OrangePortalMaterial(const OrangePortalMaterial& other) = delete;
    OrangePortalMaterial(OrangePortalMaterial&& other) noexcept = delete;
    OrangePortalMaterial& operator=(const OrangePortalMaterial& other) = delete;
    OrangePortalMaterial& operator=(OrangePortalMaterial&& other) noexcept = delete;

protected:
    void InitializeEffectVariables() override;
    void OnUpdateModelVariables(const SceneContext& context, const ModelComponent* comp) const override
    {
        // You can add custom updating code here.
        PortalMaterial::OnUpdateModelVariables(context, comp);
    }
};


class BluePortalMaterial final : public PortalMaterial
{
public:
    BluePortalMaterial();
    ~BluePortalMaterial() override = default;

    BluePortalMaterial(const BluePortalMaterial& other) = delete;
    BluePortalMaterial(BluePortalMaterial&& other) noexcept = delete;
    BluePortalMaterial& operator=(const BluePortalMaterial& other) = delete;
    BluePortalMaterial& operator=(BluePortalMaterial&& other) noexcept = delete;

protected:
    void InitializeEffectVariables() override;
    void OnUpdateModelVariables(const SceneContext& context, const ModelComponent* comp) const override
    {
        // You can add custom updating code here.
        PortalMaterial::OnUpdateModelVariables(context, comp);
    }
};
