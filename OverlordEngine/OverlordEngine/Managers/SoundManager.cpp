#include "stdafx.h"
#include "SoundManager.h"
#include <random>

SoundManager::SoundManager():
	m_MaxPitchOffset{0.12f}
{
	Initialize();
}

SoundManager::~SoundManager()
{
	if (m_pFmodSystem)
	{
#pragma warning(push)
#pragma warning(disable : 26812)
		m_pFmodSystem->release();
#pragma warning(pop)
	}
}

bool SoundManager::ErrorCheck(FMOD_RESULT res)
{
	if (res != FMOD_OK)
	{
		std::wstringstream strstr;
		strstr << L"FMOD error! \n[" << res << L"] " << FMOD_ErrorString(res) << std::endl;
		Logger::LogError(strstr.str());
		return false;
	}

	return true;
}

void SoundManager::InitializeChannelGroups()
{
	for (int i = 0; i < static_cast<int>(SoundChannel::total); ++i)
	{
		SoundChannel currentChannel = static_cast<SoundChannel>(i);

		FMOD::ChannelGroup* channelGroup = nullptr;
		FMOD_RESULT result = m_pFmodSystem->createChannelGroup(nullptr, &channelGroup);

		if (ErrorCheck(result) && channelGroup)
		{
			m_ChannelGroups[currentChannel] = channelGroup;
		}
	}
}

FMOD::Sound* SoundManager::LoadStream(const std::string& path, bool loop, bool is3D)
{
	FMOD_MODE mode = FMOD_CREATESTREAM;
	if (loop)
		mode |= FMOD_LOOP_NORMAL;
	if (is3D)
		mode |= FMOD_3D;
	else
		mode |= FMOD_2D;

	FMOD::Sound* sound;
	FMOD_RESULT result = m_pFmodSystem->createStream(path.c_str(), mode, nullptr, &sound);
	if (!ErrorCheck(result))
		return nullptr;

	return sound;
}

FMOD::Sound* SoundManager::LoadSound(const std::string& path, bool loop, bool is3D)
{
	FMOD_MODE mode = FMOD_DEFAULT;
	if (loop)
		mode |= FMOD_LOOP_NORMAL;
	else
		mode |= FMOD_LOOP_OFF;
	if (is3D)
		mode |= FMOD_3D;
	else
		mode |= FMOD_2D;

	FMOD::Sound* sound = nullptr;
	FMOD_RESULT result = m_pFmodSystem->createSound(path.c_str(), mode, nullptr, &sound);
	if (!ErrorCheck(result))
		return nullptr;

	return sound;
}

void SoundManager::Play2DSound(FMOD::Sound* sound, float volume, SoundChannel channelGroup, bool randomPitch)
{
	if (!sound) return;

	FMOD::Channel* channel;
	m_pFmodSystem->playSound(sound, m_ChannelGroups[channelGroup], false, &channel);
	channel->setVolume(volume);

	if (randomPitch && channel)
	{
		// Randomize the pitch between a range (e.g., 0.8 to 1.2 for slight variations)
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(1 - m_MaxPitchOffset, 1 + m_MaxPitchOffset);  // You can adjust these values as needed

		float pitch = static_cast<float>(dis(gen));
		channel->setPitch(pitch);
	}
}

void SoundManager::Play3DSound(FMOD::Sound* sound, float volume, const XMFLOAT3& pos, SoundChannel channelGroup, bool randomPitch)
{
	if (!sound) return;


	FMOD_VECTOR position;
	position.x = pos.x;
	position.y = pos.y;
	position.z = pos.z;


	FMOD::Channel* channel;
	m_pFmodSystem->playSound(sound, m_ChannelGroups[channelGroup], true, &channel);
	channel->setVolume(volume);

	if (channel)
	{
		channel->set3DAttributes(&position, nullptr);  // Set the position of the sound source
		if (randomPitch)
		{
			// Randomize the pitch between a range (e.g., 0.8 to 1.2 for slight variations)
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dis(1 - m_MaxPitchOffset, 1 + m_MaxPitchOffset);  // You can adjust these values as needed

			float pitch = static_cast<float>(dis(gen));
			channel->setPitch(pitch);
		}

		channel->setPaused(false);  // Play the sound after setting attributes
	}
}


void SoundManager::SetListenerAttributes(const XMFLOAT3& pos, const XMFLOAT3& vel, const XMFLOAT3& forward, const XMFLOAT3& up)
{
	FMOD_VECTOR p, v, f, u;

	// Convert XMFLOAT3 to FMOD_VECTOR for position
	p.x = pos.x;
	p.y = pos.y;
	p.z = pos.z;

	// Convert XMFLOAT3 to FMOD_VECTOR for velocity
	v.x = vel.x;
	v.y = vel.y;
	v.z = vel.z;

	// Convert XMFLOAT3 to FMOD_VECTOR for forward
	f.x = forward.x;
	f.y = forward.y;
	f.z = forward.z;

	// Convert XMFLOAT3 to FMOD_VECTOR for up
	u.x = up.x;
	u.y = up.y;
	u.z = up.z;

	m_pFmodSystem->set3DListenerAttributes(0, &p,&v,&f,&u);
}


void SoundManager::PauseChannelGroup(SoundChannel group)
{
	if (m_ChannelGroups.find(group) != m_ChannelGroups.end())
		m_ChannelGroups[group]->setPaused(true);
}

void SoundManager::ResumeChannelGroup(SoundChannel group)
{
	if (m_ChannelGroups.find(group) != m_ChannelGroups.end())
		m_ChannelGroups[group]->setPaused(false);
}

void SoundManager::StopChannelGroup(SoundChannel group)
{
	if (m_ChannelGroups.find(group) != m_ChannelGroups.end())
		m_ChannelGroups[group]->stop();
}

void SoundManager::Initialize()
{
	FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_ERROR);

	unsigned int version{};
	int numdrivers{};

	/*
	Create a System object and initialize.
	*/
	HANDLE_ERROR(System_Create(&m_pFmodSystem));
	HANDLE_ERROR(m_pFmodSystem->getVersion(&version));
	ASSERT_IF(version < FMOD_VERSION, L"Initialization Failed!\n\nYou are using an old version of FMOD {:#x}. This program requires {:#x}\n",
		version, FMOD_VERSION)

	HANDLE_ERROR(m_pFmodSystem->getNumDrivers(&numdrivers));

	if (numdrivers == 0)
	{
		HANDLE_ERROR(m_pFmodSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND));
	}
	else
	{
		HANDLE_ERROR(m_pFmodSystem->init(32, FMOD_INIT_NORMAL, nullptr));
	}

	InitializeChannelGroups();
}