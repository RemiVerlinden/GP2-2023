#pragma once

class ArrowObject final : public GameObject
{
public:
	ArrowObject(BaseMaterial* pMaterial);
	~ArrowObject() override = default;
	ArrowObject(const ArrowObject& other) = delete;
	ArrowObject(ArrowObject&& other) noexcept = delete;
	ArrowObject& operator=(const ArrowObject& other) = delete;
	ArrowObject& operator=(ArrowObject&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext&) override;

private:
	BaseMaterial* m_pMaterial{};
};

