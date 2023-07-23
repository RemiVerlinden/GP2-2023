#include "stdafx.h"
#include "ModelAnimator.h"
ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter) :
	m_pMeshFilter{ pMeshFilter }
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	if (m_IsPlaying && m_ClipSet)
	{
		// speaks for it self (the variable name, not the calculation part)
		float passedTicks = fmodf(sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed, m_CurrentClip.duration);

		// if we want to play the animation once until the end and keep it in that state
		// we have get the final tick, this one is a little tricky because the actual final tick
		// is not at the end but one tick before the end, so we have to subtract 1
		// the final tick is actually the beginning position of the animation so if we were to
		// loop the animation, everything would look smooth going from last tick to first tick
		if (float finalAnimationTick = m_CurrentClip.duration - 1; m_PlayOnce && finalAnimationTick <= ((m_TickCount + passedTicks)))
		{
			Pause();
			m_PlayOnce = false;
			m_TickCount = finalAnimationTick-0.001f;
		}

		else 
		{
			// we check if new TickCount is bigger than duration, then we just store the remainder -> duration = 12 | totalTickCount = 14 -> newTickCount = 2
			m_TickCount = fmodf((m_Reversed ? m_TickCount - passedTicks : m_TickCount + passedTicks) + m_CurrentClip.duration, m_CurrentClip.duration);
		}
	}
	AnimationKey keyA{}, keyB{};
	for (int i = 0; i < m_CurrentClip.keys.size(); ++i)
	{
		if (m_CurrentClip.keys[i].tick > m_TickCount)
		{
			keyB = m_CurrentClip.keys[i];
			keyA = m_CurrentClip.keys[i - 1];
			break;
		}
	}

	float blendFactor = (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick);

	size_t boneCount = m_CurrentClip.keys[0].boneTransforms.size();
	m_Transforms.resize(boneCount);


	for (int i = 0; i < boneCount; ++i)
	{
		XMMATRIX transformA = XMLoadFloat4x4(&keyA.boneTransforms[i]);
		XMMATRIX transformB = XMLoadFloat4x4(&keyB.boneTransforms[i]);

		XMVECTOR scaleA, rotationA, translationA, scaleB, rotationB, translationB;
		XMMatrixDecompose(&scaleA, &rotationA, &translationA, transformA);
		XMMatrixDecompose(&scaleB, &rotationB, &translationB, transformB);

		XMVECTOR scaleLerp = XMVectorLerp(scaleA, scaleB, blendFactor);
		XMVECTOR rotationSlerp = XMQuaternionSlerp(rotationA, rotationB, blendFactor);
		XMVECTOR translationLerp = XMVectorLerp(translationA, translationB, blendFactor);

		XMMATRIX resultMatrix = XMMatrixScalingFromVector(scaleLerp) * XMMatrixRotationQuaternion(rotationSlerp) * XMMatrixTranslationFromVector(translationLerp);
		XMStoreFloat4x4(&m_Transforms[i], resultMatrix);
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	m_ClipSet = false;
	for (const AnimationClip& clip : m_pMeshFilter->GetAnimationClips())
	{
		if (clip.name == clipName)
		{
			SetAnimation(clip);
			return;
		}
	}
	Reset(false);
	Logger::LogWarning(L"No animation clip has been found with clipname [" + clipName + L"]");
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	m_ClipSet = false;
	if (clipNumber < m_pMeshFilter->GetAnimationClips().size())
	{
		SetAnimation(m_pMeshFilter->GetAnimationClips().at(clipNumber));
		return;
	}
	Reset(false);
	Logger::LogWarning(L"No animation clip has been found with ID [" + std::to_wstring(clipNumber) + L"]");
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	m_ClipSet = true;
	m_CurrentClip = clip;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	if (pause)	m_IsPlaying = false;

	m_TickCount = 0;
	m_AnimationSpeed = 1.0f;

	if (m_ClipSet)
	{
		m_Transforms = m_CurrentClip.keys[0].boneTransforms;
	}
	else
	{
		const XMFLOAT4X4 identity
		{
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		};
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identity);
	}
}

void ModelAnimator::PlayOnce()
{
	Reset(false);
	m_PlayOnce = true;
	m_IsPlaying = true;
}
