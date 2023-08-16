#include "stdafx.h"
#include "Utils.h"
#include <DirectXMath.h>
#include <random>

void MatrixUtil::MultiplyMatrices(const XMFLOAT4X4& matrixA, const XMFLOAT4X4& matrixB, const XMFLOAT4X4& matrixC, XMFLOAT4X4& result)
{
	// Load matrices as XMMatrix
	XMMATRIX matA = XMLoadFloat4x4(&matrixA);
	XMMATRIX matB = XMLoadFloat4x4(&matrixB);
	XMMATRIX matC = XMLoadFloat4x4(&matrixC);

	// Multiply matrices
	XMMATRIX temp = XMMatrixMultiply(matA, matB);
	XMMATRIX finalResult = XMMatrixMultiply(temp, matC);

	// Store the result in an XMFLOAT4X4
	XMStoreFloat4x4(&result, finalResult);
}

XMFLOAT3 MatrixUtil::GetPositionFromMatrix(const XMFLOAT4X4& matrix)
{
	XMVECTOR scale, rotation, translation;

	// Load the XMFLOAT4X4 matrix into an XMMatrix
	XMMATRIX xmMatrix = XMLoadFloat4x4(&matrix);

	// Decompose the matrix
	if (XMMatrixDecompose(&scale, &rotation, &translation, xmMatrix))
	{
		XMFLOAT3 float3Translation;
		XMStoreFloat3(&float3Translation, translation);
		return float3Translation;
	}
	else
	{
		assert(false);
		// Decomposition failed; return an identity quaternion
		return XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}

XMVECTOR MatrixUtil::GetRotationFromMatrix(const XMFLOAT4X4& matrix)
{
	XMVECTOR scale, rotation, translation;

	// Load the XMFLOAT4X4 matrix into an XMMatrix
	XMMATRIX xmMatrix = XMLoadFloat4x4(&matrix);

	// Decompose the matrix
	if (XMMatrixDecompose(&scale, &rotation, &translation, xmMatrix))
	{
		return rotation;
	}
	else
	{
		assert(false);
		// Decomposition failed; return an identity quaternion
		return XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

void QuatUtil::QuaternionToEuler(const XMVECTOR& Q, float& pitch, float& yaw, float& roll)
{
	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (Q.m128_f32[3] * Q.m128_f32[2] + Q.m128_f32[0] * Q.m128_f32[1]);
	double cosy_cosp = +1.0 - 2.0 * (Q.m128_f32[1] * Q.m128_f32[1] + Q.m128_f32[2] * Q.m128_f32[2]);
	yaw = (float)atan2(siny_cosp, cosy_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (Q.m128_f32[3] * Q.m128_f32[1] - Q.m128_f32[2] * Q.m128_f32[0]);
	if (fabs(sinp) >= 1)
		pitch = (float)copysign(XM_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = (float)asin(sinp);

	// roll (x-axis rotation)
	double sinr_cosp = +2.0 * (Q.m128_f32[3] * Q.m128_f32[0] + Q.m128_f32[1] * Q.m128_f32[2]);
	double cosr_cosp = +1.0 - 2.0 * (Q.m128_f32[0] * Q.m128_f32[0] + Q.m128_f32[1] * Q.m128_f32[1]);
	roll = (float)atan2(sinr_cosp, cosr_cosp);
}

void QuatUtil::AngleVectors(const XMFLOAT3& angles, XMFLOAT3* forward, XMFLOAT3* right, XMFLOAT3* up)
{
	float sr, sp, sy, cr, cp, cy;



	MathUtil::SinCos(MathUtil::DegToRad(angles.x), &sy, &cy);
	MathUtil::SinCos(MathUtil::DegToRad(angles.y), &sp, &cp);
	MathUtil::SinCos(MathUtil::DegToRad(angles.z), &sr, &cr);

	if (forward)
	{
		forward->x = -(cr * sp * cy + -sr * -sy);
		forward->y = (sr * sp * cy + cr * -sy);
		forward->z = cp * cy;
	}

	if (right)   // This was 'right' in Source
	{
		right->x = cr * cp;
		right->y = sr * cp;
		right->z = sp;
	}

	if (up)  // This was 'up' in Source
	{
		up->x = -(cr * sp * sy + -sr * cy);
		up->z = cp * sy;
		up->y = (sr * sp * sy + cr * cy);
	}
}

float Random::GenerateRandomInterval(float min,float max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(min, max);
	return static_cast<float>(dis(gen));
}