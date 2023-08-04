#pragma once
#include "PhysX/PhysxProxy.h"
#include "Scenegraph/GameScene.h"


namespace GameSceneExt
{
	inline void CreatePhysXGroundPlane(const GameScene& scene, PxMaterial* pMaterial = nullptr)
	{
		const auto pActor = PxGetPhysics().createRigidStatic(PxTransform{ PxQuat{PxPiDivTwo, PxVec3{0.f,0.f,1.f}} });
		if (!pMaterial) pMaterial = PxGetPhysics().createMaterial(.5f, .5f, .5f);
		auto pShape = PxRigidActorExt::createExclusiveShape(*pActor, PxPlaneGeometry{}, *pMaterial);

		PxFilterData collisionGroup{};

		collisionGroup.word0 = static_cast<UINT32>((1 << 0));

		pShape->setSimulationFilterData(collisionGroup);
		pShape->setQueryFilterData(collisionGroup);

		scene.GetPhysxProxy()->AddActor(*pActor);
	}
}

namespace StringUtil
{
	// Convert a wide Unicode string to an UTF8 string
	inline std::string utf8_encode(const std::wstring& wstr)
	{
		if (wstr.empty()) return {};

		const int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], int(wstr.size()), nullptr, 0, nullptr, nullptr);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], int(wstr.size()), &strTo[0], size_needed, nullptr, nullptr);
		return strTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	inline std::wstring utf8_decode(const std::string& str)
	{
		if (str.empty()) return {};

		const int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], int(str.size()), nullptr, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], int(str.size()), &wstrTo[0], size_needed);
		return wstrTo;
	}

	// Convert an UTF8 string to a wide Unicode String
	inline std::wstring utf8_decode(const char* str)
	{
		return utf8_decode(std::string{ str });
	}

	//inline std::wstring utf8_decode(LPCSTR str)
	//{
	//	return utf8_decode(std::string(str));
	//}
}

namespace ConvertUtil
{
	inline float* ToImFloatPtr(const XMFLOAT2& v)
	{
		return reinterpret_cast<float*>(const_cast<XMFLOAT2*>(&v));
	}

	inline float* ToImFloatPtr(const XMFLOAT3& v)
	{
		return reinterpret_cast<float*>(const_cast<XMFLOAT3*>(&v));
	}

	inline float* ToImFloatPtr(const XMFLOAT4& v)
	{
		return reinterpret_cast<float*>(const_cast<XMFLOAT4*>(&v));
	}
}

namespace MatrixUtil
{
	void MultiplyMatrices(const XMFLOAT4X4& matrixA, const XMFLOAT4X4& matrixB, const XMFLOAT4X4& matrixC, XMFLOAT4X4& result);


	// all chatgpt functions I know things could be differenet but I dont care, it works so im good
	XMFLOAT3 GetPositionFromMatrix(const XMFLOAT4X4& matrix);


	XMVECTOR GetRotationFromMatrix(const XMFLOAT4X4& matrix);

}

namespace QuatUtil 
{
	void QuaternionToEuler(const XMVECTOR& Q, float& pitch, float& yaw, float& roll);

	void AngleVectors(const XMFLOAT3& angles, XMFLOAT3* forward, XMFLOAT3* right, XMFLOAT3* up);

}

namespace MathUtil
{
	// Remap a value in the range [A,B] to [C,D].
	inline float RemapVal(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return val >= B ? D : C;
		return C + (D - C) * (val - A) / (B - A);
	}

	inline float DegToRad(float degrees)
	{
		return degrees * (DirectX::XM_PI / 180.0f);
	}

	inline float RadToDeg(float radians)
	{
		return radians * (180.0f / DirectX::XM_PI);
	}

	// Math routines done in optimized assembly math package routines
	void inline SinCos(float radians, float* sine, float* cosine)
	{
		*sine = sin(radians);
		*cosine = cos(radians);
	}
}
