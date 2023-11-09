#pragma once
#include <wchar.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <exception>
#include <stdio.h>
#include <assimp/matrix4x4.h>

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
	xmMatrix.r[0] = DirectX::XMVectorSet(aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4);
	xmMatrix.r[1] = DirectX::XMVectorSet(aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4);
	xmMatrix.r[2] = DirectX::XMVectorSet(aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4);
	xmMatrix.r[3] = DirectX::XMVectorSet(aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4);
	return xmMatrix;
}

LPCWSTR GetComErrorString(HRESULT hr);

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);