#pragma once
class NoDrawMaterial final : public Material<NoDrawMaterial>
{
public:
	NoDrawMaterial();
	~NoDrawMaterial() override = default;

	NoDrawMaterial(const NoDrawMaterial& other) = delete;
	NoDrawMaterial(NoDrawMaterial&& other) noexcept = delete;
	NoDrawMaterial& operator=(const NoDrawMaterial& other) = delete;
	NoDrawMaterial& operator=(NoDrawMaterial&& other) noexcept = delete;

	void SetDebugRendering(bool setDebugDraw);
protected:
	void InitializeEffectVariables() override;

private:
	TextureData* m_pDiffuseTexture{};
};

