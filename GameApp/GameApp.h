#pragma once

#include "../Engine/CommonApp.h"

#include <d3d11.h>
#include <string>
#include <directxtk/SimpleMath.h>

class ModelLoader;
using namespace DirectX::SimpleMath;

class GameApp :
	public CommonApp
{
public:
	GameApp(HINSTANCE hInstance);
	~GameApp() = default;

	// 렌더링 파이프라인을 구성하는 필수 객체의 인터페이스 ( 뎊스 스텐실 뷰도 있지만 아직 사용하지 않는다.)
	ID3D11Device* m_pDevice = nullptr;						// 디바이스	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;	// 깊이값 처리를 위한 뎊스스텐실 뷰

	// 렌더링 파이프라인에 적용하는 객체와 정보
	ID3D11VertexShader* m_pVertexShader = nullptr;	// 정점 셰이더
	ID3D11PixelShader* m_pPixelShader = nullptr;	// 픽셀 셰이더
	ID3D11InputLayout* m_pInputLayout = nullptr;		// 입력 레이아웃
	ID3D11Buffer* m_pTransformConstantBuffer = nullptr;		// 위치 상수 버퍼
	ID3D11Buffer* m_pLightingConstantBuffer = nullptr;	// 조명 상수 버퍼
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// 샘플러 상태

	// 모델 로더
	ModelLoader* m_pModelLoader = nullptr;
	std::string m_ModelPath = "../Resources/FBX/zeldaPosed001.fbx";

	// 상수 버퍼를 통해 전달할 변환 정보
	// 위치 상수 버퍼
	Matrix m_WorldModel;
	Matrix m_View;
	Matrix m_Projection;

	// 조명 상수 버퍼
	Vector4 m_LightDirection = { 0.f, 0.f, 1.f, 1.f };
	Vector4 m_LightColor = { 1.f, 1.f, 1.f, 1.f };
	FLOAT m_LightIntensity = 1.f;
	FLOAT m_AmbientPower = 0.2f;
	FLOAT m_SpecularPower = 2000.f;

	// ImGUI와 연결할 정보
	Vector3 m_TranslateModel = { 0.f, 0.f, 0.f };
	Vector3 m_TranslateCamera = { 0.f, 0.f, -500.f };
	FLOAT m_FOV = 45.f;	// XM_PIDIV4
	FLOAT m_NearZ = 0.1f;
	FLOAT m_FarZ = 10000.f;
	FLOAT m_CameraRotation = 0.f;
	FLOAT m_ModelYAW = 0.f;
	FLOAT m_ModelPitch = 0.f;
	
	bool Initialize() final;
	void Update() final;
	void Render() final;
	void Finalize() final;

private:
	bool InitializeD3D();
	void FinalizeD3D();

	bool InitializeScene();	// 셰이더, 버텍스, 인덱스
	void FinalizeScene();

	bool InitializeImGUI();
	void FinalizeImGUI();

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};
