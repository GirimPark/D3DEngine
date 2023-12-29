#include "pch.h"
#include "Helper.h"
#include <comdef.h>
#include <d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

LPCWSTR GetComErrorString(HRESULT hr)
{
	_com_error err(hr);
	LPCWSTR errMsg = err.ErrorMessage();
	return errMsg;
}

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, LPCSTR modelType)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS; 
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging, ����ȭ �ǳʶٱ�
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	// D3DCompileFromFile
	// (���ϸ�, ���̴� ��ũ��, �����Ϸ��� ���� ������ ó���ϴ� ������(include Shared.fxh), ������ �Լ� �̸�, ���̴� ��,
	//	���̴� ������ �ɼ�, ȿ�� ������ �ɼ�, �����ϵ� �ڵ忡 �׼����� ������ ����, �����Ϸ� ���� �޽����� �׼����� ������ ����)
	D3D10_SHADER_MACRO defines[] =
	{
		{modelType, ""},
		{nullptr, nullptr}
	};
	hr = D3DCompileFromFile(szFileName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if(FAILED(hr))
	{
		if(pErrorBlob)
		{
			MessageBoxA(NULL, (char*)pErrorBlob->GetBufferPointer(), "
				", MB_OK);
			pErrorBlob->Release();
		}
		return hr;
	}

	if(pErrorBlob)
	{
		pErrorBlob->Release();
	}

	return S_OK;
}