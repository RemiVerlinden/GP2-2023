#pragma once
class Character; // You need to include this if you want to use Character* in your class
#include <DirectXMath.h>
class ViewBob
{
public:
    ViewBob(Character* character);
    float CalcViewmodelBob(float dt);
    void AddViewmodelBob(DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& angles);
    void CalcViewModelLag(float dt, DirectX::XMFLOAT3& origin, DirectX::XMFLOAT3& angles,const DirectX::XMFLOAT3& originalAngles );

private:
    float g_lateralBob;
    float g_verticalBob;
    float bobtime;
    float lastbobtime;
    Character* character;
};
