#pragma once

class ModelTestScene final : public GameScene
{
public:
	ModelTestScene();
	~ModelTestScene() override = default;

	ModelTestScene(const ModelTestScene& other) = delete;
	ModelTestScene(ModelTestScene&& other) noexcept = delete;
	ModelTestScene& operator=(const ModelTestScene& other) = delete;
	ModelTestScene& operator=(ModelTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;
	void OnSceneActivated() override;
private:
	void InitializeGroundPlane();
	void InitializeChairObject();
	void ResetChairPosRot();
	ImVec4 LerpColors(const ImVec4& colorA, const ImVec4& colorB, float t);

	GameObject* m_pChair;

};


