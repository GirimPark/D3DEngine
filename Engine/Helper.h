#pragma once
#include <wchar.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <exception>
#include <stdio.h>
#include <assimp/matrix4x4.h>
#include <directxtk/SimpleMath.h>

#define LOG_ERROR(...) \
{ \
	wchar_t buffer[256]; \
	swprintf_s(buffer, 256, L"[ERROR] %s:%d - ", __FUNCTIONW__, __LINE__); \
	wchar_t message[256]; \
	swprintf_s(message, 256, __VA_ARGS__); \
	wcscat_s(buffer, message); \
	wcscat_s(buffer, L"\n"); \
	MessageBoxW(NULL, buffer, L"LOG_ERROR", MB_OK); \
}

#define LOG_WARNING(...) \
{ \
	wchar_t buffer[256]; \
    swprintf_s(buffer,256, L"[WARNING] %s:%d - ", __FUNCTIONW__, __LINE__); \
    wchar_t message[256]; \
    swprintf_s(message,256, __VA_ARGS__); \
    wcscat_s(buffer, message); \
    wcscat_s(buffer, L"\n"); \
    OutputDebugStringW(buffer); \
}

#define LOG_MESSAGE(...) \
{ \
    wchar_t buffer[256]; \
    swprintf_s(buffer,256, L"[MESSAGE] %s:%d - ", __FUNCTIONW__, __LINE__); \
    wchar_t message[256]; \
    swprintf_s(message,256, __VA_ARGS__); \
    wcscat_s(buffer, message); \
    wcscat_s(buffer, L"\n"); \
    OutputDebugStringW(buffer); \
}


template <typename T>
void SAFE_RELEASE(T* p)
{
	if(p)
	{
        p->Release();
        p = nullptr;
	}
}

template <typename T>
void SAFE_DELETE(T* p)
{
	if(p)
	{
		delete p;
		p = nullptr;
	}
}


class com_exception : public std::exception
{
public:
	com_exception(HRESULT hr) : result(hr) {}

	const char* what() const noexcept override
	{
		static char s_str[64] = {};
		sprintf_s(s_str, "Failure with HRESULT of %08x",
			static_cast<unsigned int>(result));

		return s_str;
	}

private:
	HRESULT result;
};

inline void HR_T(HRESULT hr)
{
	if(FAILED(hr))
	{
		throw com_exception(hr);
	}
}


inline DirectX::XMMATRIX ConvertaiMatrixToXMMatrix(const aiMatrix4x4 aiMatrix)
{
	DirectX::XMMATRIX xmMatrix;
	xmMatrix.r[0] = DirectX::XMVectorSet(aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1);
	xmMatrix.r[1] = DirectX::XMVectorSet(aiMatrix.a2, aiMatrix.b2, aiMatrix.c2, aiMatrix.d2);
	xmMatrix.r[2] = DirectX::XMVectorSet(aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3);
	xmMatrix.r[3] = DirectX::XMVectorSet(aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4);
	return xmMatrix;
}

inline DirectX::XMVECTOR GetPositionVector(const DirectX::XMMATRIX matrix)
{
	return matrix.r[3];
}

inline DirectX::XMVECTOR GetScaleVector(const DirectX::XMMATRIX matrix)
{
	DirectX::XMVECTOR scaleVector;
	scaleVector.m128_f32[0] = DirectX::XMVector3Length(matrix.r[0]).m128_f32[0];
	scaleVector.m128_f32[1] = DirectX::XMVector3Length(matrix.r[1]).m128_f32[1];
	scaleVector.m128_f32[2] = DirectX::XMVector3Length(matrix.r[2]).m128_f32[2];

	return scaleVector;
}

inline DirectX::SimpleMath::Quaternion GetQuaternion(const DirectX::XMMATRIX matrix)
{
	DirectX::XMMATRIX rotationMatrix = matrix;
	rotationMatrix.r[3] = DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f);
	DirectX::XMVECTOR tempScale = GetScaleVector(matrix);
	DirectX::XMVECTOR scale = { tempScale.m128_f32[0], tempScale.m128_f32[1], tempScale.m128_f32[2] };
	rotationMatrix = DirectX::XMMatrixScalingFromVector(scale) * DirectX::XMMatrixTranspose(rotationMatrix);
	DirectX::XMFLOAT4 rotationQuaternion;
	DirectX::XMStoreFloat4(&rotationQuaternion, DirectX::XMQuaternionRotationMatrix(rotationMatrix));

	return rotationQuaternion;
}

LPCWSTR GetComErrorString(HRESULT hr);

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, LPCSTR modelType);