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
	XMFLOAT3 position;
	position.x = matrix._41;
	position.y = matrix._42;
	position.z = matrix._43;
	return position;
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
		// Decomposition failed; return an identity quaternion
		return XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	}
}
