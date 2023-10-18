#pragma once
class AnimationBlender final
{
public:
	AnimationBlender(ModelAnimator* pAnimator)
		: m_pAnimator{ pAnimator }
		, m_BlendDuration{ 0.05f }
		, m_BlendTime{ 0 }
		, m_Blending{ false }
		, m_PreviousTickCount{ 0 }
    {}

    void StartBlend(const AnimationClip& newClip)
    {
        m_PreviousClip = m_pAnimator->m_CurrentClip; // save the last clip
        m_PreviousTickCount = m_PreviousClip.ticksPerSecond; // save the last progress
        m_NewClip = newClip;
        m_Blending = true;
        m_BlendTime = 0.0f;
    }

    void Update(float elapsedSec)
    {
		if (m_Blending)
		{
			m_BlendTime += elapsedSec;
			if (m_BlendTime > m_BlendDuration)
			{
				// end the blend, set the new clip to the animator
				m_pAnimator->SetAnimation(m_NewClip);
				m_Blending = false;
			}
			else
			{
				// calculate blend factor
				float blendFactor = m_BlendTime / m_BlendDuration;

				// Get the end frame of the previous clip
				//AnimationKey endKeyPrevClip = m_PreviousClip.keys[m_PreviousClip.keys.size() - 1];
				AnimationKey keyPrevClip;
				for (const auto& key : m_PreviousClip.keys)
				{
					if (key.tick < m_PreviousTickCount)
					{
						keyPrevClip = key;
						
					}
					else if (key.tick > m_PreviousTickCount)
					{
						break;
					}
				}

				// Get the start frame of the new clip
				//AnimationKey startKeyNewClip = m_NewClip.keys[0];

				float timeInOldClip = m_BlendTime * m_PreviousClip.ticksPerSecond;
				float timeInNewClip = m_BlendTime * m_NewClip.ticksPerSecond;
				// Get the interpolated frame at that time in the new clip
				AnimationKey interpolatedKeyOldClip = GetInterpolatedKey(m_PreviousClip, timeInOldClip);
				AnimationKey interpolatedKeyNewClip = GetInterpolatedKey(m_NewClip, timeInNewClip);

				// Interpolate between frames of m_PreviousClip and m_NewClip
				// This is similar to the interpolation done in ModelAnimator::Update, but here you use the blendFactor for lerp/slerp functions
				// Make sure to use appropriate interpolation method for your specific data structure
				for (size_t i = 0; i < m_pAnimator->m_pMeshFilter->GetBoneCount(); i++)
				{
					//auto transformA = endKeyPrevClip.boneTransforms[i];
					//auto transformA = keyPrevClip.boneTransforms[i];
					auto transformA = interpolatedKeyOldClip.boneTransforms[i];

					//auto transformB = startKeyNewClip.boneTransforms[i];
					auto transformB = interpolatedKeyNewClip.boneTransforms[i];

					//	Decompose both transforms
					XMVECTOR scaleA, rotQuatA, transA;
					XMVECTOR scaleB, rotQuatB, transB;

					XMMatrixDecompose(&scaleA, &rotQuatA, &transA, XMLoadFloat4x4(&transformA));
					XMMatrixDecompose(&scaleB, &rotQuatB, &transB, XMLoadFloat4x4(&transformB));

					//	Lerp between all the transformations (Position, Scale, Rotation)
					auto lerpScale = XMVectorLerp(scaleA, scaleB, blendFactor);
					auto lerpRotQuat = XMQuaternionSlerp(rotQuatA, rotQuatB, blendFactor);
					auto lerpTrans = XMVectorLerp(transA, transB, blendFactor);

					//	Compose a transformation matrix with the lerp-results
					XMMATRIX newTransform = XMMatrixAffineTransformation(lerpScale, XMVectorZero(), lerpRotQuat, lerpTrans);

					//	Add the resulting matrix to the m_Transforms vector
					XMFLOAT4X4 result;
					XMStoreFloat4x4(&result, newTransform);

					// Add the resulting matrix to the m_Animator's m_Transforms vector
					m_pAnimator->m_Transforms[i] = result;
				}
			}
		}
    }

    AnimationKey GetInterpolatedKey(const AnimationClip& clip, float time)
    {
        AnimationKey interpolatedKey;
        interpolatedKey.tick = time;

        // Handle looping
        time = fmod(time, clip.duration);

		// Update ModelAnimator
		if (m_pAnimator->m_Reversed)
		{
			m_pAnimator->m_TickCount -= time;
			if (m_pAnimator->m_TickCount < 0)
			{
				m_pAnimator->m_TickCount += clip.duration;
			}
		}
		else
		{
			m_pAnimator->m_TickCount += time;
			if (m_pAnimator->m_TickCount > clip.duration)
			{
				m_pAnimator->m_TickCount -= clip.duration;
			}
		}

        // Find the enclosing keys
        AnimationKey keyA, keyB;
        // Iterate all the keys of the clip and find the following keys:
        // keyA > Closest Key with Tick before/smaller than time
        // keyB > Closest Key with Tick after/bigger than time
        for (const auto& key : clip.keys)
        {
			if (key.tick <= time)
			{
				keyA = key;
			}
			else if (key.tick >= time)
			{
				keyB = key;
				break;
			}
			else
				return interpolatedKey;
        }

        // Figure out the BlendFactor
        float blendFactor = (time - keyA.tick) / (keyB.tick - keyA.tick);

        // Interpolate between keys
        // For every boneTransform in a key (So for every bone)
        for (size_t i = 0; i < m_pAnimator->m_pMeshFilter->GetBoneCount(); i++)
        {
            // Retrieve the transform from keyA (transformA)
            auto transformA = keyA.boneTransforms[i];

            // Retrieve the transform from keyB (transformB)
            auto transformB = keyB.boneTransforms[i];

            // Decompose both transforms
            XMVECTOR scaleA, rotQuatA, transA;
            XMVECTOR scaleB, rotQuatB, transB;

            XMMatrixDecompose(&scaleA, &rotQuatA, &transA, XMLoadFloat4x4(&transformA));
            XMMatrixDecompose(&scaleB, &rotQuatB, &transB, XMLoadFloat4x4(&transformB));

            // Lerp between all the transformations (Position, Scale, Rotation)
            auto lerpScale = XMVectorLerp(scaleA, scaleB, blendFactor);
            auto lerpRotQuat = XMQuaternionSlerp(rotQuatA, rotQuatB, blendFactor);
            auto lerpTrans = XMVectorLerp(transA, transB, blendFactor);

            // Compose a transformation matrix with the lerp-results
            XMMATRIX newTransform = XMMatrixAffineTransformation(lerpScale, XMVectorZero(), lerpRotQuat, lerpTrans);

            // Add the resulting matrix to the interpolatedKey boneTransforms vector
            XMFLOAT4X4 result;
            XMStoreFloat4x4(&result, newTransform);
            interpolatedKey.boneTransforms.push_back(result);
        }

        return interpolatedKey;
    }

    bool IsBlending() { return m_Blending; }

private:
	ModelAnimator* m_pAnimator{ nullptr };
    AnimationClip m_PreviousClip;
    AnimationClip m_NewClip;
	float m_PreviousTickCount;
	float m_BlendTime;
	float m_BlendDuration;
	bool m_Blending;
};