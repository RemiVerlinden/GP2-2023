#pragma once
class TestScene final : public GameScene
{
public:
	TestScene();
	~TestScene() override = default;

	TestScene(const TestScene& other) = delete;
	TestScene(TestScene&& other) noexcept = delete;
	TestScene& operator=(const TestScene& other) = delete;
	TestScene& operator=(TestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	GameObject* m_pTorusBig{nullptr};
	GameObject* m_pTorusMiddle{ nullptr };
	GameObject* m_pTorusSmall{ nullptr };
	
};


