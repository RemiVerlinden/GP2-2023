#pragma once
#include "UberMaterial.h"
class SkullBallUberMaterial final : public UberMaterial
{
public:
	SkullBallUberMaterial();
	~SkullBallUberMaterial() override = default;

	SkullBallUberMaterial(const SkullBallUberMaterial& other) = delete;
	SkullBallUberMaterial(SkullBallUberMaterial&& other) noexcept = delete;
	SkullBallUberMaterial& operator=(const SkullBallUberMaterial& other) = delete;
	SkullBallUberMaterial& operator=(SkullBallUberMaterial&& other) noexcept = delete;

private:
	void InitializeEffectVariables() override;
};