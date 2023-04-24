#pragma once
struct GameSettings
{
	GameSettings() = default;

	int width{1280};
	int height{720};

	std::wstring windowTitle{L"GP2 - Milestone 1 (2023) | (2DAE07) Verlinden Remi"};
	bool isVSyncOn{true};
	XMFLOAT3 clearColor{ Colors::CornflowerBlue };

	float GetAspectRatio() const
	{
		return width / float(height);
	}
};

