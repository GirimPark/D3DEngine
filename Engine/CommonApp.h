#pragma once

#include <Windows.h>

#define MAX_LOADSTRING (100)

class CommonApp
{
protected:
	CommonApp(HINSTANCE hInstance);
	virtual ~CommonApp() = default;

public:
	static HWND m_hWnd;
	static CommonApp* m_pInstance;	// 매 프레임 돌아야하기 때문에 public으로 선언

protected:
	HACCEL m_hAccelTable;
	MSG m_msg;
	HINSTANCE m_hInstance;
	WCHAR m_szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
	WCHAR m_szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
	WNDCLASSEXW m_wcex;
	int m_nCmdShow;

public:
	virtual bool Initialize();
	void Run();
	virtual void Update();
	virtual void Render() abstract;
	virtual void Finalize();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

