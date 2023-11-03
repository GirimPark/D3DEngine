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

	// ������ ������������ �����ϴ� �ʼ� ��ü�� �������̽� ( �X�� ���ٽ� �䵵 ������ ���� ������� �ʴ´�.)
	ID3D11Device* m_pDevice = nullptr;						// ����̽�	
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;					// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;	// ���̰� ó���� ���� �X�����ٽ� ��

	// ������ ���������ο� �����ϴ� ��ü�� ����
	ID3D11VertexShader* m_pVertexShader = nullptr;	// ���� ���̴�
	ID3D11PixelShader* m_pPixelShader = nullptr;	// �ȼ� ���̴�
	ID3D11InputLayout* m_pInputLayout = nullptr;		// �Է� ���̾ƿ�
	ID3D11Buffer* m_pTransformConstantBuffer = nullptr;		// ��ġ ��� ����
	ID3D11Buffer* m_pLightingConstantBuffer = nullptr;	// ���� ��� ����
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// ���÷� ����

	// �� �δ�
	ModelLoader* m_pModelLoader = nullptr;
	std::string m_ModelPath = "../Resources/FBX/zeldaPosed001.fbx";

	// ��� ���۸� ���� ������ ��ȯ ����
	// ��ġ ��� ����
	Matrix m_WorldModel;
	Matrix m_View;
	Matrix m_Projection;

	// ���� ��� ����
	Vector4 m_LightDirection = { 0.f, 0.f, 1.f, 1.f };
	Vector4 m_LightColor = { 1.f, 1.f, 1.f, 1.f };
	FLOAT m_LightIntensity = 1.f;
	FLOAT m_AmbientPower = 0.2f;
	FLOAT m_SpecularPower = 2000.f;

	// ImGUI�� ������ ����
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

	bool InitializeScene();	// ���̴�, ���ؽ�, �ε���
	void FinalizeScene();

	bool InitializeImGUI();
	void FinalizeImGUI();

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};
