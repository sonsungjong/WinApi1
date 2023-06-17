#include "framework.h"
#include "WinApi2.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    WinApp win(L"my_sample_class_name", hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    
    win.Create(L"타이틀 네임 123 !");
    return 0;
}

// 생성자
WinApp::WinApp(const wchar_t* ap_class_name, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    m_hInst = hInstance;
    m_class_name = ap_class_name;
    m_cmd_show = nCmdShow;
}

WinApp::~WinApp()
{
}

LRESULT WinApp::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL WinApp::OnCreate(HWND ah_wnd, LPCREATESTRUCT lpCreateStruct)
{
    BaseWindow::OnCreate(ah_wnd, lpCreateStruct);

    m_button1 = CreateWindowW(L"Button", L"버튼 윈도우 생성", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 100, 200, 24, m_hWnd, (HMENU)IDC_BUTTON1, m_hInst, NULL);
    m_edit1 = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 95, 200, 30, m_hWnd, (HMENU)IDC_EDIT1, m_hInst, NULL);


    return TRUE;
}


void WinApp::OnDestroy(HWND ah_wnd)
{
    PostQuitMessage(0);
}

void WinApp::OnCommand(HWND ah_wnd, int a_id, HWND ah_wnd_ctrl, UINT codeNotify)
{
    if (a_id == IDC_BUTTON1) {
        wchar_t str_width[20] = { 0, };
        wchar_t str_height[20] = { 0, };
        swprintf_s(str_width, 19, L"가로크기 : %d", m_rect.right - m_rect.left);
        swprintf_s(str_height, 19, L"세로크기 : %d", m_rect.bottom - m_rect.top);
        MessageBox(m_hWnd, str_width, str_height, MB_OK);
    }
    else if (a_id == IDCANCEL) {
        OnDestroy(m_hWnd);
    }
}

void WinApp::OnPaint(HWND ah_wnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hWnd, &ps);
    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
    EndPaint(m_hWnd, &ps);
}