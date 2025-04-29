#include "framework.h"
#include "TreeView1.h"
#include <CommCtrl.h>

HINSTANCE g_hInst;
HWND g_hWnd;

HWND g_TreeView;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
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
    g_hInst = hInstance;                    // 인스턴스 핸들을 전역 변수에 저장합니다.

    // Register Class
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = g_hInst;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(250,250,250));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Test1";
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    // 창의 크기를 지정
    RECT rc = { 0, 0, 640, 480 };
    ::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);              // 여백보정

    g_hWnd = ::CreateWindowW(L"Test1", L"Blank Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, g_hInst, nullptr);

    ShowWindow(g_hWnd, true);
    UpdateWindow(g_hWnd);

    // 단축키 테이블
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TREEVIEW1));

    g_TreeView = CreateWindowEx(
        NULL, /*WS_EX_CLIENTEDGE,*/
        L"SysTreeView32",
        TEXT(""),
        WS_CHILD | WS_VISIBLE | /*WS_BORDER |*/ WS_VSCROLL | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_TABSTOP,
        30, 30, 300, 400,
        g_hWnd, (HMENU)IDC_MY_TREE,
        g_hInst, NULL
    );

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        // DirectX는 항상 렌더링 될 수 있게 PeekMessage를 사용해야한다
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 메시지 처리
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // 엔진 렌더링 수행
            
        }
    }

    return (int)msg.wParam;
}


void testTreeItems(HWND hWndTree)
{

}