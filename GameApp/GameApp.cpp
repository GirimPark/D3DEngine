#include "framework.h"
#include "GameApp.h"

#include <directxtk/SimpleMath.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#pragma comment (lib, "d3d11.lib")

#define USE_FLIPMODE 1

using namespace DirectX;
using namespace DirectX::SimpleMath;

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

struct Vertex
{
	Vector3 position;
	Vector4 color;

	Vertex(float x, float y, float z) : position(x, y, z) { }
	Vertex(Vector3 position) : position(position) { }

	Vertex(Vector3 position, Vector4 color)
		: position(position), color(color) { }
};

struct ConstantBuffer
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
};


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

	if(!InitializeScene())
	{
		return false;
	}

	if(!InitializeImGUI())
	{
		return false;
	}

	return true;
}

void GameApp::Update()
{
	static float angle = 0.f;
	angle += 0.002f;

	// Sun
	XMMATRIX spinS = XMMatrixRotationY(angle);
	XMMATRIX translateS = XMMatrixTranslation(m_TranslateSun.x, m_TranslateSun.y, m_TranslateSun.z);
	m_WorldSun = spinS * translateS;

	// Earth
	XMMATRIX spinE = XMMatrixRotationY(angle * 2.f);
	XMMATRIX orbitE = m_WorldSun;
	XMMATRIX translateE = XMMatrixTranslation(m_TranslateEarth.x, m_TranslateEarth.y, m_TranslateEarth.z);
	XMMATRIX scaleE = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	m_WorldEarth = scaleE * spinE * translateE * orbitE;

	// Moon
	XMMATRIX spinM = XMMatrixRotationY(angle * 4.f);
	XMMATRIX orbitM = m_WorldEarth;
	XMMATRIX translateM = XMMatrixTranslation(m_TranslateMoon.x, m_TranslateMoon.y, m_TranslateMoon.z);
	XMMATRIX scaleM = XMMatrixScaling(0.4f, 0.4f, 0.4f);
	m_WorldMoon = scaleM * spinM * translateM * orbitM;

	// Camera
	XMVECTOR Eye = XMVectorSet(m_TranslateCamera.x, m_TranslateCamera.y, m_TranslateCamera.z, 0.f);
	XMVECTOR At = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR Up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	m_Projection = XMMatrixPerspectiveFovLH(DEGREE_TO_RADIAN(m_FOV), ScreenWidth / static_cast<FLOAT>(ScreenHeight), m_NearZ, m_FarZ);

	__super::Update();
}

void GameApp::Render()
{
	/// ImGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Edit Transform");
		ImGui::SliderFloat3("Sun", (float*)&m_TranslateSun, -50.f, 50.f);
		ImGui::SliderFloat3("Earth", (float*)&m_TranslateEarth, -50.f, 50.f);
		ImGui::SliderFloat3("Moon", (float*)&m_TranslateMoon, -50.f, 50.f);
		ImGui::SliderFloat3("Camera", (float*)&m_TranslateCamera, -50.f, 50.f);
		ImGui::SliderFloat("FOV", &m_FOV, 0.1f, 180.f);
		ImGui::SliderFloat("Near", &m_NearZ, 0.01f, 100.f);
		ImGui::SliderFloat("Far", &m_FarZ, 100.f, 1000.f);

		ImGui::End();
	}

	ImGui::Render();

#if USE_FLIPMODE == 1
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
#endif

	// 화면, Depth Buffer 초기화
	DirectX::SimpleMath::Color color(0.f, 0.5f, 0.5f, 1.0f);
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

	// Draw 계열 함수를 호출하기 전 렌더링 파이프라인에 필수 스테이지 설정을 해야한다.
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 정점을 이어서 그리는 방식
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

	// Sun : Update Variables
	ConstantBuffer cbSun;
	cbSun.mWorld = XMMatrixTranspose(m_WorldSun);
	cbSun.mView = XMMatrixTranspose(m_View);
	cbSun.mProjection = XMMatrixTranspose(m_Projection);
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cbSun, 0, 0);
	// Render
	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

	// Earth : Update Variables
	ConstantBuffer cbEarth;
	cbEarth.mWorld = XMMatrixTranspose(m_WorldEarth);
	cbEarth.mView = XMMatrixTranspose(m_View);
	cbEarth.mProjection = XMMatrixTranspose(m_Projection);
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cbEarth, 0, 0);
	// Render
	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

	// Earth : Update Variables
	ConstantBuffer cbMoon;
	cbEarth.mWorld = XMMatrixTranspose(m_WorldMoon);
	cbEarth.mView = XMMatrixTranspose(m_View);
	cbEarth.mProjection = XMMatrixTranspose(m_Projection);
	m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cbEarth, 0, 0);
	// Render
	m_pDeviceContext->DrawIndexed(m_nIndices, 0, 0);

	/// ImGUI
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Present the information rendered to the back buffer to the front buffer (the screen)
	m_pSwapChain->Present(0, 0);
}

void GameApp::Finalize()
{
	FinalizeD3D();
	FinalizeScene();
	FinalizeImGUI();
}

bool GameApp::InitializeD3D()
{
	// 결과값
	HRESULT hr = 0;


	/// 1. 스왑체인 속성 설정 구조체 생성
	// https://learn.microsoft.com/ko-kr/windows/win32/api/dxgi/ns-dxgi-dxgi_swap_chain_desc
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
#if USE_FLIPMODE == 1
	swapDesc.BufferCount = 2;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// Present()호출 후 디스플레이 화면에서 픽셀을 처리하는 옵션
#else
	swapDesc.BufferCount = 1;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
#endif

	// BufferDesc : 백 버퍼 표시 모드
	// BufferDesc - 백버퍼(텍스처)의 가로/세로 크기 설정
	swapDesc.BufferDesc.Width = ScreenWidth;
	swapDesc.BufferDesc.Height = ScreenHeight;
	// BufferDesc - 화면 주사율 결정
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// BufferDesc - 표시 형식
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 다중 샘플링 매개 변수 설정
	swapDesc.SampleDesc.Count = 1;		// 다중 샘플링 하지 않음
	swapDesc.SampleDesc.Quality = 0;	// 다중 샘플링 하지 않음

	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;
	swapDesc.Windowed = true;


	/// 2. Device, DeviceContext, SwapChain 생성
	// https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/nf-d3d11-d3d11createdeviceandswapchain
	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));


	/// 3. RenderTargetView 생성
	// https://learn.microsoft.com/ko-kr/windows/win32/api/d3d11/nf-d3d11-id3d11device-createrendertargetview
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView));	// 텍스처 내부 참조 증가
	SAFE_RELEASE(pBackBufferTexture);

#if USE_FLIPMODE == 0
	// 렌더 타겟을 최종 출력 파이프라인에 바인딩
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
#endif


	/// 4. 뷰포트 설정
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(ScreenWidth);
	viewport.Height = static_cast<float>(ScreenHeight);
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_pDeviceContext->RSSetViewports(1, &viewport);


	/// 5. Depth&Stencil View 생성
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = ScreenWidth;
	depthDesc.Height = ScreenHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	ID3D11Texture2D* textureDepthStencil = nullptr;
	HR_T(m_pDevice->CreateTexture2D(&depthDesc, nullptr, &textureDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
	DSVDesc.Format = depthDesc.Format;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0;
	HR_T(m_pDevice->CreateDepthStencilView(textureDepthStencil, &DSVDesc, &m_pDepthStencilView));
	SAFE_RELEASE(textureDepthStencil);

	return true;
}

void GameApp::FinalizeD3D()
{
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDevice);
}

bool GameApp::InitializeScene()
{
	HRESULT hr = 0;
	ID3D10Blob* errorMessage = nullptr;	// 컴파일 에러 메시지가 저장될 버퍼

	///1. Render() 에서 파이프라인에 바인딩할 버텍스 버퍼및 버퍼 정보 준비

	Vertex vertices[] =
	{
		{ Vector3(-1.0f, 1.0f, -1.0f),	Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, -1.0f),	Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),	Vector4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f),	Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, -1.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, -1.0f),	Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, 1.0f),	Vector4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ Vector3(-1.0f, -1.0f, 1.0f),	Vector4(0.0f, 0.0f, 0.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC vbDesc = {};
	m_VertexCount = ARRAYSIZE(vertices);
	vbDesc.ByteWidth = sizeof(Vertex) * m_VertexCount;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.CPUAccessFlags = 0;

	// 정점 버퍼 생성
	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;
	HR_T(hr = m_pDevice->CreateBuffer(&vbDesc, &vbData, &m_pVertexBuffer));

	// 버텍스 버퍼 정보
	m_VertexBufferStride = sizeof(Vertex);
	m_VertexBufferOffset = 0;


	/// 2. Render() 에서 파이프라인에 바인딩할 InputLayout 생성
	// 인풋 레이아웃은 버텍스 쉐이더가 입력받을 데이터의 형식을 지정한다.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ID3DBlob* vertexShaderBuffer = nullptr;
	HR_T(CompileShaderFromFile(L"BasicVertexShader.hlsl", "main", "vs_4_0", &vertexShaderBuffer));
	HR_T(hr = m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_pInputLayout));


	/// 3. Render에서 파이프라인에 바인딩할 버텍스 셰이더 생성
	HR_T(m_pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_pVertexShader));
	SAFE_RELEASE(vertexShaderBuffer);


	/// 4. Render()에서 파이프라인에 바인딩할 인덱스 버퍼 생성
	WORD indices[] =
	{
		0,5,4, 0,1,5,
		1,6,5, 1,2,6,
		2,7,6, 2,3,7,
		3,4,7, 3,0,4,
		3,1,0, 3,2,1,
		5,7,4, 5,6,7
	};

	// 인덱스 개수 저장
	m_nIndices = ARRAYSIZE(indices);
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(WORD) * m_nIndices;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices;
	HR_T(m_pDevice->CreateBuffer(&ibDesc, &ibData, &m_pIndexBuffer));


	/// 5. Render에서 파이프라인에 바인딩할 픽셀 셰이더 생성
	ID3DBlob* pixelShaderBuffer = nullptr;
	HR_T(CompileShaderFromFile(L"BasicPixelShader.hlsl", "main", "ps_4_0", &pixelShaderBuffer));
	HR_T(m_pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pPixelShader));
	SAFE_RELEASE(pixelShaderBuffer);


	/// 6. Render()에서 파이프라인에 바인딩할 상수 버퍼 생성
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.ByteWidth = sizeof(ConstantBuffer);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pConstantBuffer));

	// 쉐이더에 전달할 데이터 설정
	m_WorldSun = XMMatrixIdentity();

	XMVECTOR Eye = XMVectorSet(m_TranslateCamera.x, m_TranslateCamera.y, m_TranslateCamera.z, 0.f);
	XMVECTOR At = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR Up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	m_View = XMMatrixLookAtLH(Eye, At, Up);

	m_Projection = XMMatrixPerspectiveFovLH(m_FOV, ScreenWidth / static_cast<FLOAT>(ScreenHeight), m_NearZ, m_FarZ);

	return true;
}

void GameApp::FinalizeScene()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pDepthStencilView);
}

bool GameApp::InitializeImGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(this->m_pDevice, this->m_pDeviceContext);

	return true;
}

void GameApp::FinalizeImGUI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT GameApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	return __super::WndProc(hWnd, message, wParam, lParam);
}