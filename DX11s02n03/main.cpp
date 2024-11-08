// DX11s02n03.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "main.h"
#include <d3d11.h>
#include <dxgi.h>
#include "DemoBlankWndD3D.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// 전역 변수:
HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
HWND g_hWnd;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                CreateMainWnd(int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    g_hInst = hInstance;
    g_hWnd = NULL;
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!CreateMainWnd(nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX11S02N03));

    // D3D Tester //
    CDemoBlankWndD3D demo;
    if (!demo.Initialize(g_hWnd, g_hInst)) {
        return -1;
    }

    // 기본 메시지 루프입니다:
    MSG msg = {0};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // D3D Update and Render //
        demo.Update();
        demo.Render();
    }

    // D3D Termiate //
    demo.Terminate();

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = NULL;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = L"BlankWndClass";
    wcex.hIconSm        = NULL;

    return RegisterClassExW(&wcex);
}

BOOL CreateMainWnd(int nCmdShow)
{
    RECT rc = { 0, 0, 640, 480 };
    ::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

   g_hWnd = ::CreateWindowW(L"BlankWndClass", L"Blank Window", 
       WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 
       rc.right - rc.left, rc.bottom-rc.top,
       nullptr, nullptr, g_hInst, nullptr);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_PAINT:    
        hdc = ::BeginPaint(hWnd, &ps);
        ::EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
