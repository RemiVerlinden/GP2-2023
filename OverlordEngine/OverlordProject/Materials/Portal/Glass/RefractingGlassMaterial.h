#pragma once
class RefractingGlassMaterial : public Material<RefractingGlassMaterial>
{
public:
	RefractingGlassMaterial();
	~RefractingGlassMaterial() override = default;

	RefractingGlassMaterial(const RefractingGlassMaterial& other) = delete;
	RefractingGlassMaterial(RefractingGlassMaterial&& other) noexcept = delete;
	RefractingGlassMaterial& operator=(const RefractingGlassMaterial& other) = delete;
	RefractingGlassMaterial& operator=(RefractingGlassMaterial&& other) noexcept = delete;

protected:
	void InitializeEffectVariables() override;

private:
};