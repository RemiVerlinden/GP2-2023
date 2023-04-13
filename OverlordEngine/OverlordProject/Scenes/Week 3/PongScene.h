#pragma once
#include <utility> // for std::pair

class PongScene final : public GameScene
{
public:
	PongScene();
	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;
	virtual void OnSceneActivated();

private:
	void InitializePaddleInput();
	void MoveCube(GameObject* prefab, float distance);
	void ResetBall();

	struct BallProperties
	{
		float					speed					= 11.f;
		float					radius					= 1;
		XMFLOAT3				spawnPos				= { 0, radius, 0 };
		XMFLOAT4				color					= XMFLOAT4{ Colors::Red };
	} BallProps;

	struct PaddleProperties
	{
		float					speed					= 15.f ;
		XMFLOAT3				size					= { 1,1,4 };
		XMFLOAT3				centerSpawnPos			= { 0, size.y / 2, 0 };
		float					horizontalSpawnOffset	= { 15 };
		XMFLOAT4				color					= XMFLOAT4{ Colors::Blue  };
	} PaddleProps;

	struct LevelBoundsProperties
	{
		PxVec3					levelBounds				= { 44,0,25 };
		XMFLOAT3				centerPos				= { 0, 0, 0 };
		float					boundsBoxWidth			= 1.f;
		float					boundsBoxHeight			= 2.f;
	} LevelBoundsProps;


	enum PongInput : int8_t
	{
		PLAYER1_UP,
		PLAYER1_DOWN,
		PLAYER2_UP,
		PLAYER2_DOWN,
		STARTGAME
	};

private:
	std::pair<GameObject*, GameObject*> m_Paddles;
	GameObject* m_pBall;
	RigidBodyComponent* m_pBallRigidBody;
	FreeCamera* m_pFixedCamera{};
	bool m_GameStarted = false;
};