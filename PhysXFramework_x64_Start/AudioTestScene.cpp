#include "stdafx.h"
#include "AudioTestScene.h"
#include "PhysxManager.h"
#include "SpherePosColorNorm.h"
#include "Logger.h"
#include "SoundManager.h"

void AudioTestScene::Initialize()
{
	EnablePhysxDebugRendering(true);

	PxPhysics* pPhysX = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = pPhysX->createMaterial(1.f, 1.f, .2f);

	// Ground plane
	auto pGroundActor = pPhysX->createRigidStatic(PxTransform{ {PxPiDivTwo,{0,0,1}} });
	PxRigidActorExt::createExclusiveShape(*pGroundActor, PxPlaneGeometry{}, *pDefaultMaterial);
	m_pPhysxScene->addActor(*pGroundActor);

	// Sphere
	m_pSphere = new SpherePosColorNorm(1, 10, 10, XMFLOAT4{ Colors::Gray });
	AddGameObject(m_pSphere);

	auto pSphereActor = pPhysX->createRigidDynamic(PxTransform{ PxIdentity });
	PxRigidActorExt::createExclusiveShape(*pSphereActor, PxSphereGeometry{ 1.f }, *pDefaultMaterial);

	m_pSphere->AttachRigidActor(pSphereActor);
	m_pSphere->Translate(0, 2, 0);

	// SOUND 2D
	auto pFmod = SoundManager::GetInstance()->GetSystem();

	FMOD::Sound* pSound{};
	FMOD_RESULT result = pFmod->createStream("Resources/Sounds/Wave.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &pSound);
	SoundManager::GetInstance()->ErrorCheck(result);

	result = pFmod->playSound(pSound, nullptr, true, &m_pChannel2D);
	SoundManager::GetInstance()->ErrorCheck(result);

	// SOUND 3D
	result = pFmod->createStream("Resources/Sounds/Thrones.mp3", FMOD_3D | FMOD_3D_LINEARROLLOFF, nullptr, &pSound);
	SoundManager::GetInstance()->ErrorCheck(result);


	result = pFmod->playSound(pSound, nullptr, true, &m_pChannel3D);
	SoundManager::GetInstance()->ErrorCheck(result);

	m_pChannel3D->setMode(FMOD_LOOP_NORMAL);
	m_pChannel3D->set3DMinMaxDistance(0, 100);

	// INPUT 
	auto pInput = m_SceneContext.GetInput();
	pInput->AddInputAction({ play2D,		 InputTriggerState::pressed, VK_SPACE });
	pInput->AddInputAction({ play3D,		 InputTriggerState::pressed, 'B'});
	pInput->AddInputAction({ increaseVolume, InputTriggerState::pressed, VK_UP });
	pInput->AddInputAction({ decreaseVolume, InputTriggerState::pressed, VK_DOWN});
}

inline FMOD_VECTOR ToFmod(XMFLOAT3 v)
{
	return FMOD_VECTOR{v.x,v.y,v.z};
}

inline FMOD_VECTOR ToFmod(PxVec3 v)
{
	return FMOD_VECTOR{v.x,v.y,v.z};
}

void AudioTestScene::Update()
{
	float force = 5.f;
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_LEFT))
	{
		// only rigidDynamic can have torque applied to we have to tell it specifically that we are
		// dealing with a rigid DYNAMIC actor
		m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->addTorque({ 0,0,force });
	}
	if (m_SceneContext.GetInput()->IsKeyboardKey(InputTriggerState::down, VK_RIGHT))
	{
		// only rigidDynamic can have torque applied to we have to tell it specifically that we are
		// dealing with a rigid DYNAMIC actor
		m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->addTorque({ 0,0,-force });
	}

	bool		play2D = m_SceneContext.GetInput()->IsActionTriggered(InputIds::play2D);
	bool		play3D = m_SceneContext.GetInput()->IsActionTriggered(InputIds::play3D);
	bool		increaseVolume = m_SceneContext.GetInput()->IsActionTriggered(InputIds::increaseVolume);
	bool		decreaseVolume = m_SceneContext.GetInput()->IsActionTriggered(InputIds::decreaseVolume);

	if (play2D)
	{
		bool bPaused = false;
		m_pChannel2D->getPaused(&bPaused);
		m_pChannel2D->setPaused(!bPaused);
	}
	if (play3D)
	{
		bool bPaused = false;
		m_pChannel3D->getPaused(&bPaused);
		m_pChannel3D->setPaused(!bPaused);
	}	
	if (increaseVolume)
	{
		float volume{};
		m_pChannel2D->getVolume(&volume);
		volume += 0.1f;
		Clamp(volume, 1.f,0.f);
		m_pChannel2D->setVolume(volume);
		Logger::GetInstance()->LogFormat(LogLevel::Info, L"Sound Volume Changed > %f", volume);
	}	
	if (decreaseVolume)
	{
		float volume{};
		m_pChannel2D->getVolume(&volume);
		volume -= 0.1f;
		Clamp(volume, 1.f,0.f);
		m_pChannel2D->setVolume(volume);
		Logger::GetInstance()->LogFormat(LogLevel::Info, L"Sound Volume Changed > %f", volume);
	}

	// 3D SOUND CONFIG
	// Listener
	const auto pCam = m_SceneContext.GetCamera();
	FMOD_VECTOR pos = ToFmod(pCam->GetPosition());
	FMOD_VECTOR forward = ToFmod(pCam->GetForward());
	FMOD_VECTOR up = ToFmod(pCam->GetUp());

	FMOD_VECTOR vel{};
	auto timeStep = m_SceneContext.GetGameTime()->GetElapsed();
	vel.x = (pos.x - m_PreviousListenerPos.x) / timeStep;
	vel.y = (pos.y - m_PreviousListenerPos.y) / timeStep;
	vel.z = (pos.z - m_PreviousListenerPos.z) / timeStep;
	m_PreviousListenerPos = pos;
	SoundManager::GetInstance()->GetSystem()->set3DListenerAttributes(0, &pos, &vel, &forward, &up);

	// Source
	auto spherePos = ToFmod(m_pSphere->GetPosition());
	auto sphereVel = ToFmod(m_pSphere->GetRigidActor()->is<PxRigidDynamic>()->getLinearVelocity());
	m_pChannel3D->set3DAttributes(&spherePos,&sphereVel);
}

void AudioTestScene::Draw() const
{
}
