#pragma once
#include <vector>
class GunScene final : public GameScene
{
public:
	GunScene();
	~GunScene() override = default;

	GunScene(const GunScene& other) = delete;
	GunScene(GunScene&& other) noexcept = delete;
	GunScene& operator=(const GunScene& other) = delete;
	GunScene& operator=(GunScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;
private:
	void InitializeGunObject();
	void AddModel(std::wstring assetfile);


	std::vector<GameObject*> m_pModelsVec;

};


