#include "framework.h"
#include "D2DBasic1.h"

#include "DrawRectangle.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "WindowsCodecs.lib")

// GPU : unsigned int, float

HINSTANCE g_hInst;                                // current instance
HWND g_hWnd;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    if (SUCCEEDED(CoInitialize(NULL))) {
        DemoApp app;
        if (SUCCEEDED(app.init(hInstance))) {
            app.runMessageLoop();
        }
        CoUninitialize();
    }

    return 0;
}

//LRESULT onPaint(HWND _hWnd)
//{
//    PAINTSTRUCT ps;
//    BeginPaint(_hWnd, &ps);
//
//    ::RECT rc;
//    GetClientRect(_hWnd, &rc);
//    ::FillRect(ps.hdc, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));
//
//    HPEN hMyRedPen = ::CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
//    HGDIOBJ hOldPen = ::SelectObject(ps.hdc, hMyRedPen);
//    ::Rectangle(ps.hdc, rc.left + 100, rc.top + 100, rc.right - 100, rc.bottom - 100);
//
//    ::SelectObject(ps.hdc, hOldPen);
//    ::DeleteObject(hMyRedPen);
//
//    EndPaint(_hWnd, &ps);
//    return 0;
//}
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    case WM_LBUTTONDOWN:
//        MessageBox(0, L"Hello World", L"Hello", MB_OK);
//        break;
//    case WM_KEYDOWN:
//        if (wParam == VK_ESCAPE) {
//            DestroyWindow(g_hWnd);
//        }
//        break;
//    case WM_PAINT:
//        onPaint(g_hWnd);
//        break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}
//
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//                     _In_opt_ HINSTANCE hPrevInstance,
//                     _In_ LPWSTR    lpCmdLine,
//                     _In_ int       nCmdShow)
//{
//    g_hInst = hInstance;
//
//    WNDCLASS wc;
//    wc.style = CS_HREDRAW | CS_VREDRAW;
//    wc.lpfnWndProc = (WNDPROC)WndProc;
//    wc.cbClsExtra = 0;
//    wc.cbWndExtra = 0;
//    wc.hInstance = hInstance;
//    wc.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
//    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//    wc.lpszMenuName = 0;
//    wc.lpszClassName = L"Hello";
//    RegisterClass(&wc);
//    g_hWnd = ::CreateWindow(L"Hello", L"Hello Window", WS_OVERLAPPEDWINDOW,
//        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//        NULL, NULL, g_hInst, NULL);
//
//    ShowWindow(g_hWnd, nCmdShow);
//    UpdateWindow(g_hWnd);
//
//    MSG msg = {};
//    while (msg.message != WM_QUIT) {
//        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//        else {
//            //render();
//        }
//    }
//
//    return (int) msg.wParam;
//}

