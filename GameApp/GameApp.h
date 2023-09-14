#pragma once

#include "../Engine/CommonApp.h"

#include <d3d11.h>
#include <directxtk/SimpleMath.h>

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
	ID3D11Buffer* m_pVertexBuffer = nullptr;		// ���ؽ� ����
	UINT m_VertexBufferStride = 0;					// ���ؽ� �ϳ��� ũ��
	UINT m_VertexBufferOffset = 0;					// ���ؽ� ������ ������
	UINT m_VertexCount = 0;							// ���ؽ� ����
	ID3D11Buffer* m_pIndexBuffer = nullptr;			// �ε��� ����
	int m_nIndices = 0;								// �ε��� ����
	ID3D11Buffer* m_pConstantBuffer = nullptr;		// ��� ����

	// ImGUI Test
	Vector4 m_ClearColor;

	// ��� ���۸� ���� ������ ��ȯ ����
	Matrix m_WorldSun;
	Matrix m_WorldEarth;
	Matrix m_WorldMoon;
	Matrix m_View;
	Matrix m_Projection;

	
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
