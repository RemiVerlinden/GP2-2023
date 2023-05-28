#include "stdafx.h"
#include "Utils.h"
#include <DirectXMath.h>

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

