#include "pch.h"
#include "framework.h"
#include "D2DButtonSample1.h"
#include "D2DButtonClass.h"

HINSTANCE g_hInst;
HWND g_hWnd;
ID2D1Factory* g_d2dFactory = nullptr;
IDWriteFactory* g_dwriteFactory = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static D2DButton* button = nullptr;

    switch (message)
    {
    case WM_CREATE:
        // Direct2D 및 DirectWrite 팩토리 초기화
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2dFactory);
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&g_dwriteFactory);

        // D2DButton 생성 및 리소스 초기화
        button = new D2DButton(hWnd, 50, 50, 200, 50, L"클릭하세요");
        button->InitializeResources(g_d2dFactory, g_dwriteFactory);
        button->SetClickCallback([]() {
            ::MessageBox(g_hWnd, L"버튼이 클릭되었습니다.", L"알림", MB_OK);
            });
        break;

    case WM_COMMAND:
        if (button && lParam == (LPARAM)button->GetHandle()) {
            button->OnClick();
        }
        break;

    case WM_PAINT:
        if (button) {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            button->Draw(); // 버튼 그리기 호출
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        delete button;
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
        g_dwriteFactory->Release();
        g_dwriteFactory = nullptr;
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    g_hInst = hInstance;

    // 윈도우 클래스 등록
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = g_hInst;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Test1";
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    // 메인 윈도우 생성
    RECT rc = { 0, 0, 640, 480 };
    ::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    g_hWnd = ::CreateWindowW(L"Test1", L"Direct2D Button Example",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, g_hInst, nullptr);

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // DirectX 엔진 렌더링 수행
        }
    }

    return (int)msg.wParam;
}