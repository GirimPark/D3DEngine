#include "framework.h"
#include "GameApp.h"

#include <directxtk/SimpleMath.h>

#include "../Engine/define.h"

#pragma comment (lib, "d3d11.lib")

#define USE_FLIPMODE 1

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Debug Memory Leak Check at start point
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(2214);

	// 전역 문자열을 초기화합니다.
	GameApp App(hInstance);
	App.Initialize();
	App.Run();

	// Debug Memory Leak Check at end point
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtDumpMemoryLeaks();

	return (int)1;
}


GameApp::GameApp(HINSTANCE hInstance)
	: CommonApp(hInstance)
{
}

bool GameApp::Initialize()
{
	__super::Initialize();

	if(!InitializeD3D())
	{
		return false;
	}

	return true;
}

void GameApp::Update()
{
	__super::Update();
}

void GameApp::Render()
{
#if USE_FLIPMODE == 1
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
#endif

	// 화면 칠하기
	DirectX::SimpleMath::Color color(0.f, 0.5f, 0.5f, 1.0f);
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);

	// 스왑체인
	m_pSwapChain->Present(0, 0);
}

void GameApp::Finalize()
{
	FinalizeD3D();
}

bool GameApp::InitializeD3D()
{
	// 결과값
	HRESULT hr = 0;

	// 스왑체인 속성 설정 구조체 생성
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
#if USE_FLIPMODE == 1
	swapDesc.BufferCount = 2;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
#else
	swapDesc.BufferCount = 1;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
#endif

	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;
	swapDesc.Windowed = true;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 백버퍼(텍스처)의 가로/세로 크기 설정
	swapDesc.BufferDesc.Width = ScreenWidth;
	swapDesc.BufferDesc.Height = ScreenHeight;
	// 화면 주사율 결정
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// 샘플링 관련 설정
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT createFlags = 0;

#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Device, DeviceContext, SwapChain 생성
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);

	ID3D11Texture2D* pBackBufferTexture = nullptr;
	if(SUCCEEDED(hr))
	{
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture);
	}
	if(SUCCEEDED(hr))
	{
		// 텍스쳐 내부 참조 증가
		hr = m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView);
	}
	// Release
	pBackBufferTexture->Release();
	pBackBufferTexture = nullptr;

#if USE_FLIPMODE == 0
	// 렌더 타겟을 최종 출력 파이프라인에 바인딩
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
#endif

	return true;
}

void GameApp::FinalizeD3D()
{
	m_pRenderTargetView->Release();
	m_pRenderTargetView = nullptr;

	m_pDeviceContext->Release();
	m_pDeviceContext = nullptr;

	m_pSwapChain->Release();
	m_pSwapChain = nullptr;

	m_pDevice->Release();
	m_pDevice = nullptr;
}
