#pragma once

#include <d3d11.h>
#include "../Engine/CommonApp.h"

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

	bool Initialize() final;
	void Update() final;
	void Render() final;
	void Finalize() final;

private:
	bool InitializeD3D();
	void FinalizeD3D();
};