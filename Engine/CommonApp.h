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
	static CommonApp* m_pInstance;	// �� ������ ���ƾ��ϱ� ������ public���� ����

protected:
	HACCEL m_hAccelTable;
	MSG m_msg;
	HINSTANCE m_hInstance;
	WCHAR m_szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
	WCHAR m_szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
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

