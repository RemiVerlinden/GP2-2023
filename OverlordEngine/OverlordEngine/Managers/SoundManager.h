#pragma once
class SoundManager final: public Singleton<SoundManager>
{
public:
	SoundManager(const SoundManager& other) = delete;
	SoundManager(SoundManager&& other) noexcept = delete;
	SoundManager& operator=(const SoundManager& other) = delete;
	SoundManager& operator=(SoundManager&& other) noexcept = delete;

	FMOD::System* GetSystem() const { return m_pFmodSystem; }
	bool ErrorCheck(FMOD_RESULT res);

public:
	enum class SoundChannel 
	{
		Music = 0,
		Background,
		Portals,
		Level,
		Weapon,
		total
	};
	FMOD::Sound* LoadStream(const std::string& path, bool loop = false, bool is3D = false);
	FMOD::Sound* LoadSound(const std::string& path, bool loop = false, bool is3D = false);
	void Play2DSound(FMOD::Sound* sound,float volume, SoundChannel channelGroup, bool randomPitch = false);
	void Play3DSound(FMOD::Sound* sound, float volume, const XMFLOAT3& position, SoundChannel channelGroup, bool randomPitch = false);

	void SetListenerAttributes(const XMFLOAT3& pos, const XMFLOAT3& vel, const XMFLOAT3& forward, const XMFLOAT3& up);

	void PauseChannelGroup(SoundChannel groupName);
	void ResumeChannelGroup(SoundChannel groupName);
	void StopChannelGroup(SoundChannel groupName);

protected:
	void Initialize() override;

private:
	void InitializeChannelGroups();

	friend class Singleton<SoundManager>;
	SoundManager();
	~SoundManager();

	float m_MaxPitchOffset = 0.12f;

	FMOD::System* m_pFmodSystem = nullptr;
	std::map<SoundChannel, FMOD::ChannelGroup*> m_ChannelGroups;
};

