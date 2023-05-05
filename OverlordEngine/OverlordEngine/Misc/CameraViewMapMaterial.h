#pragma once
class CameraViewMapMaterial final: public Material<CameraViewMapMaterial>
{
public:
	CameraViewMapMaterial();
	~CameraViewMapMaterial() override = default;
	CameraViewMapMaterial(const CameraViewMapMaterial& other) = delete;
	CameraViewMapMaterial(CameraViewMapMaterial&& other) noexcept = delete;
	CameraViewMapMaterial& operator=(const CameraViewMapMaterial& other) = delete;
	CameraViewMapMaterial& operator=(CameraViewMapMaterial&& other) noexcept = delete;

protected:
	void InitializeEffectVariables() override;
};

