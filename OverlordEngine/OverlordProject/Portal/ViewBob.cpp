#include "stdafx.h"
#include "ViewBob.h"
#include "Prefabs\Character.h"
#include <DirectXMath.h>
#include <algorithm>
#include "Utils\Utils.h"

#define BOB_CYCLE_MIN 1.0f
#define BOB_CYCLE_MAX 0.8f
#define BOB_UP 0.5f
#define M_PI 3.14159265358979323846f


ViewBob::ViewBob(Character* character)
    : character(character), bobtime(0.0f), lastbobtime(0.0f), g_lateralBob(0.0f), g_verticalBob(0.0f)
{
}

float ViewBob::CalcViewmodelBob(float currentTime)
{
    const float maxspeed = CharacterDesc::maxMoveSpeed;

    float cycle;

    DirectX::XMFLOAT2 velocityFloat2 = { character->GetVelocity().x, character->GetVelocity().z };
    float speed = sqrt(velocityFloat2.x * velocityFloat2.x + velocityFloat2.y * velocityFloat2.y); // get the magnitude of the 2D vector

    speed = std::clamp(speed, -maxspeed, maxspeed); // max and min speed values based on original code

    // Remap the speed value between 0 and 1
    float bob_offset = MathUtil::RemapVal(speed, 0, maxspeed, 0.0f, 1.0f);

    bobtime += (currentTime - lastbobtime) * bob_offset;
    lastbobtime = currentTime;

    cycle = bobtime - (int)(bobtime / BOB_CYCLE_MAX) * BOB_CYCLE_MAX;
    cycle /= BOB_CYCLE_MAX;

    if (cycle < BOB_UP)
    {
        cycle = M_PI * cycle / BOB_UP;
    }
    else
    {
        cycle = M_PI + M_PI * (cycle - BOB_UP) / (1.0f - BOB_UP);
    }

    g_verticalBob = speed * 0.005f;
    g_verticalBob = g_verticalBob * 0.3f + g_verticalBob * 0.7f * sin(cycle);
    g_verticalBob = std::clamp(g_verticalBob, -7.0f, 4.0f);


    cycle = bobtime - (int)(bobtime / BOB_CYCLE_MAX * 2) * BOB_CYCLE_MAX * 2;
    cycle /= BOB_CYCLE_MAX * 2;

    if (cycle < BOB_UP)
    {
        cycle = M_PI * cycle / BOB_UP;
    }
    else
    {
        cycle = M_PI + M_PI * (cycle - BOB_UP) / (1.0f - BOB_UP);
    }


    g_lateralBob = speed * 0.005f;
    g_lateralBob = g_lateralBob * 0.3f + g_lateralBob * 0.7f * sin(cycle);
    g_lateralBob = std::clamp(g_lateralBob, -7.0f, 4.0f);

    return 0.0f;
}

void ViewBob::AddViewmodelBob(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& angles)
{
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    DirectX::XMFLOAT3 bobUp, bobRight, bobForward;

    DirectX::XMVECTOR g_verticalBobVector = DirectX::XMVectorReplicate(g_verticalBob);
    DirectX::XMVECTOR g_lateralBobVector = DirectX::XMVectorReplicate(g_lateralBob);

    DirectX::XMStoreFloat3(&bobUp, DirectX::XMVectorMultiply(up, DirectX::XMVectorScale(g_verticalBobVector, 0.2f)));
    DirectX::XMStoreFloat3(&bobRight, DirectX::XMVectorMultiply(right, DirectX::XMVectorScale(g_lateralBobVector, 0.8f)));
    DirectX::XMStoreFloat3(&bobForward, DirectX::XMVectorMultiply(forward, DirectX::XMVectorScale(g_verticalBobVector, 0.1f)));

    origin = DirectX::XMFLOAT3(origin.x + bobRight.x + bobForward.x,
        origin.y + bobUp.y,
        origin.z + bobRight.z + bobForward.z);

    angles.x -= g_verticalBob * 0.4f;
    angles.y -= g_verticalBob * 0.3f;
    angles.z += g_lateralBob * 0.5f;
}


float g_fMaxViewModelLag = 0.17f;
float g_fViewModelLagScale = 0.17f;

void ViewBob::CalcViewModelLag(float dt, DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& angles,const DirectX::XMFLOAT3& original_angles)
{
    float pitch, yaw;

    DirectX::XMFLOAT3 vOriginalOrigin = origin;
    DirectX::XMFLOAT3 vOriginalAngles = angles;

    DirectX::XMVECTOR forward;
    DirectX::XMVECTOR up;
    DirectX::XMVECTOR right;

    
   pitch = MathUtil::DegToRad(angles.x);
   yaw = MathUtil::DegToRad(angles.y);

    // Get the forward direction

    XMFLOAT3 f, u, r;
    QuatUtil::AngleVectors(angles, &f, &r, &u);

    forward = XMLoadFloat3(&f);

    if (dt != 0.0f)
    {
        static XMVECTOR m_vecLastFacing = XMLoadFloat3(&f);
        // Calculate the difference
        DirectX::XMVECTOR vDifference = DirectX::XMVectorSubtract(forward, m_vecLastFacing);


        float flSpeed = 5.f;

        // If we start to lag too far behind, we'll increase the "catch up" speed.
        float flDiff = DirectX::XMVectorGetX(DirectX::XMVector3Length(vDifference));
        if ((flDiff > g_fMaxViewModelLag) && (g_fMaxViewModelLag > 0.0f))
        {
            float flScale = flDiff / g_fMaxViewModelLag;
            flSpeed *= flScale;
        }

        m_vecLastFacing = DirectX::XMVectorAdd(m_vecLastFacing, DirectX::XMVectorScale(vDifference, flSpeed * dt));
        m_vecLastFacing = DirectX::XMVector3Normalize(m_vecLastFacing);

        DirectX::XMVECTOR newOrigin = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&origin), DirectX::XMVectorScale(vDifference, g_fViewModelLagScale));
        origin = DirectX::XMFLOAT3(DirectX::XMVectorGetX(newOrigin), DirectX::XMVectorGetY(newOrigin), DirectX::XMVectorGetZ(newOrigin));
    }

    // Set up the rotation matrix with original_angles
    QuatUtil::AngleVectors(original_angles, &f, &r, &u);

    // Get the directions
    forward = XMLoadFloat3(&f);
    right = XMLoadFloat3(&r);
    up = XMLoadFloat3(&u);

    pitch = original_angles.x;
    if (pitch > 180.0f)
        pitch -= 360.0f;
    else if (pitch < -180.0f)
        pitch += 360.0f;

    if (g_fMaxViewModelLag == 0.0f)
    {
        origin = vOriginalOrigin;
        angles = vOriginalAngles;
    }
}