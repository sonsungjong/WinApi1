#include "pch.h"
#include "main.h"

HINSTANCE g_hInst;
HWND g_hWnd;
int g_wndWidth;
int g_wndHeight;

HANDLE g_hSerial;
std::vector<POINT> g_points;
std::mutex g_mtx;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    g_hInst = hInstance;

    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(243, 243, 243));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"LZR920Class";
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    // 초기화
    if (!openSerialPort()) {
        MessageBox(NULL, _T("시리얼 포트 초기화 실패"), _T("ERROR"), MB_ICONERROR);
        return -1;
    }

    // 영역 계산
    RECT rcClient = { 0, 0, 800, 800 };
    AdjustWindowRect(&rcClient, WS_POPUP | WS_THICKFRAME, FALSE);

    g_hWnd = ::CreateWindowW(wcex.lpszClassName, L"LZR920Class",
        WS_POPUP | WS_THICKFRAME,
        rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
        NULL, NULL, hInstance, NULL);

    RECT rc_wnd;
    if (GetClientRect(g_hWnd, &rc_wnd)) {
        g_wndWidth = rc_wnd.right - rc_wnd.left;
        g_wndHeight = rc_wnd.bottom - rc_wnd.top;
    }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LZR920));

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        // DX는 항상 렌더링 될 수 있게 PeekMessage를 사용해야한다
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 메시지 처리
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                //PreTranslateMessage(&msg);
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // 렌더링
            
        }
    }

    return (int)msg.wParam;
}

BOOL PreTranslateMessage(MSG* pMsg)
{

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (WM_PAINT == msg)
    {
        OnPaint(hWnd);
    }
    else if (WM_NCHITTEST == msg)
    {
        // 창을 드래그할 수 있게 변경
        return HTCAPTION;
    }
    else if (WM_COMMAND == msg)
    {
        int id = LOWORD(wParam);                        // 명령ID
        HWND hwndCtrl = (HWND)lParam;           // 명령을 발생시킨 컨트롤 핸들 (없으면 NULL)
        UINT codeNotify = HIWORD(wParam);       // 알림 코드
        OnCommand(hWnd, id, hwndCtrl, codeNotify);
    }
    else if (WM_TIMER == msg)
    {
        UINT_PTR timerID = (UINT_PTR)wParam;                    // 타이머 식별자
        OnTimer(hWnd, timerID);
    }
    else if (WM_CLOSE == msg)
    {
        OnClose(hWnd);
    }
    else if (WM_DESTROY == msg)
    {
        OnDestroy(hWnd);
    }
    else if (WM_CREATE == msg)
    {
        OnCreate(hWnd, (LPCREATESTRUCT)lParam);
    }
    else
    {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    DrawGDI(hdc);

    EndPaint(hWnd, &ps);
}

int OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    std::thread(recvSerialPort).detach();

    return 0;
}

void OnDestroy(HWND hWnd)
{
    // 리소스 정리


    PostQuitMessage(0);
}

void OnClose(HWND hWnd)
{
    int nResult = MessageBoxW(hWnd, L"프로그램을 종료하시겠습니까?", L"종료 확인", MB_OKCANCEL | MB_ICONQUESTION);
    if (nResult == IDOK)
    {
        DestroyWindow(hWnd);
    }
}

void OnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT codeNotify)
{
    switch (id)
    {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
    }
}

void OnTimer(HWND hWnd, UINT_PTR id)
{

}

// L"\\\\.\\COM8"
// 460800
bool openSerialPort(const wchar_t* portName, DWORD baudRate)
{
    bool bResult = false;
    g_hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (g_hSerial != INVALID_HANDLE_VALUE) {
        DCB dcb = { 0 };
        dcb.DCBlength = sizeof(DCB);
        BOOL state = GetCommState(g_hSerial, &dcb);
        if (TRUE == state)
        {
            dcb.BaudRate = baudRate;
            dcb.ByteSize = 8;
            dcb.Parity = NOPARITY;
            dcb.StopBits = ONESTOPBIT;
            SetCommState(g_hSerial, &dcb);

            // 타임아웃 설정
            COMMTIMEOUTS timeouts = { 10, 1, 10, 0, 0 };
            SetCommTimeouts(g_hSerial, &timeouts);

            bResult = true;
        }
    }

    return bResult;
}

void recvSerialPort()
{
    unsigned char buf[1024]{};
    DWORD bytesRead;

    // 시작부터 끝까지 수신받는다

}

void DrawGDI(HDC hdc)
{
    std::lock_guard<std::mutex> lock(g_mtx);
    for (const auto& pt : g_points) {
        Ellipse(hdc, pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
    }
}
