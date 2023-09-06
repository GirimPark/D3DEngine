#include "pch.h"
#include "CommonApp.h"

#ifdef ENGINE_DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

CommonApp* CommonApp::m_pInstance = nullptr;
HWND CommonApp::m_hWnd;

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return CommonApp::m_pInstance->WndProc(hWnd, message, wParam, lParam);
}

CommonApp::CommonApp(HINSTANCE hInstance)
    : m_hInstance(hInstance)
    , m_szWindowClass(L"DefaultWindowClass")
    , m_szTitle(L"CommonApp")
{
    CommonApp::m_pInstance = this;

    m_wcex.hInstance = hInstance;
    m_wcex.cbSize = sizeof(WNDCLASSEX);
    m_wcex.style = CS_HREDRAW | CS_VREDRAW;
    m_wcex.lpfnWndProc = DefaultWndProc;
    m_wcex.cbClsExtra = 0;
    m_wcex.cbWndExtra = 0;
    m_wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    m_wcex.lpszClassName = m_szWindowClass;
}

bool CommonApp::Initialize()
{
    // ���
    RegisterClassExW(&m_wcex);

    RECT rcClinet = { 0, 0, ScreenWidth, ScreenHeight };
	AdjustWindowRect(&rcClinet, WS_OVERLAPPEDWINDOW, FALSE);

    // ������ ȭ�� �߾ӿ� ����
    const int screenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2;
    const int screenCenterY = GetSystemMetrics(SM_CYSCREEN) / 2;
    const int startX = screenCenterX - ScreenWidth / 2;
    const int startY = screenCenterY - ScreenHeight / 2;

    // ������ ����
    m_hWnd = CreateWindowW(m_szWindowClass, m_szTitle, WS_OVERLAPPEDWINDOW,
        startX, startY,                       // ���� ��ġ
        rcClinet.right - rcClinet.left, rcClinet.bottom - rcClinet.top,                     // ����, ����
        nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hWnd)
    {
        return false;
    }

    SetForegroundWindow(m_hWnd);    // �����츦 �ֻ�����
    SetFocus(m_hWnd);               // �����쿡 ��Ŀ���� �ش�
    ShowCursor(TRUE);	            // Ŀ�� ���̱�

    // ������ ���̱�
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return true;
}

void CommonApp::Run()
{
    while (true)
    {
        if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
        {
            if (m_msg.message == WM_QUIT)
                break;

            if (!TranslateAccelerator(m_msg.hwnd, m_hAccelTable, &m_msg))
            {
                TranslateMessage(&m_msg);
                DispatchMessage(&m_msg);
            }
        }
        else
        {
            Update();
            Render();
        }
    }

    Finalize();
}

void CommonApp::Update()
{
}

void CommonApp::Finalize()
{
}

LRESULT CommonApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
