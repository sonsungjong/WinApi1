#pragma once

#include "resource.h"

class BaseWindow {
public:
    static INT_PTR CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        BaseWindow* app = NULL;

        if (msg == WM_NCCREATE) {
            app = (BaseWindow*)(((CREATESTRUCT*)lParam)->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(app));
            app->m_hWnd = hWnd;
        }
        else {
            app = (BaseWindow*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (app) {
            return app->HandleMessage(hWnd, msg, wParam, lParam);
        }
        else {
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }

    BaseWindow() : m_hWnd(NULL) {}
    virtual ~BaseWindow() {}

    BOOL Create(const wchar_t* wnd_title_name, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT, DWORD dwStyle = WS_OVERLAPPEDWINDOW, DWORD dwExStyle = NULL, HWND hWndParent = NULL, HMENU hMenu = NULL)
    {
        WNDCLASS wc = { 0 };

        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();
        wc.lpszMenuName = NULL;
        wc.style = CS_HREDRAW | CS_VREDRAW;

        RegisterClass(&wc);

        m_hWnd = CreateWindowEx( dwExStyle, ClassName(), wnd_title_name, dwStyle, x, y, width, height, hWndParent, hMenu, GetModuleHandle(NULL), this);
        if (m_hWnd != NULL) {
            ShowWindow(m_hWnd, CmdShow());
            UpdateWindow(m_hWnd);

            MSG msg = {};
            while (GetMessageW(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            return TRUE;
        }
        return FALSE;
    }

    HWND m_hWnd = NULL;
    HINSTANCE m_hInst = NULL;
    HWND Window() const { return m_hWnd; }
    HINSTANCE Instance() const { return m_hInst; }
    RECT m_rect;

    virtual BOOL OnCreate(HWND ah_wnd, LPCREATESTRUCT lpCreateStruct) { return GetClientRect(ah_wnd, &m_rect); }
    virtual void OnDestroy(HWND ah_wnd) { PostQuitMessage(0); }
    virtual void OnPaint(HWND ah_wnd) {}
    virtual void OnCommand(HWND ah_wnd, int a_id, HWND ah_wnd_ctrl, UINT codeNotify) {}
    virtual void OnLButtonDown(HWND ah_wnd, BOOL a_double_click, int a_x, int a_y, UINT a_key_flags) {}
    virtual void OnLButtonUp(HWND ah_wnd, BOOL a_double_click, int x, int y, UINT a_key_flags) {}
    virtual void OnMouseMove(HWND ah_wnd, int x, int y, UINT a_key_flags) {}
    virtual void OnRButtonDown(HWND ah_wnd, BOOL a_double_click, int a_x, int a_y, UINT a_key_flags) {}
    virtual void OnRButtonUp(HWND ah_wnd, BOOL a_double_click, int x, int y, UINT a_key_flags) {}
    virtual void OnMButtonDown(HWND ah_wnd, BOOL a_double_click, int a_x, int a_y, UINT a_key_flags) {}
    virtual void OnLButtonDblClk(HWND ah_wnd, BOOL a_double_click, int x, int y, UINT a_key_flags) {}
    virtual void OnSize(HWND ah_wnd, UINT state, int cx, int cy) {}
    virtual void OnClose(HWND ah_wnd) {}
    virtual void OnTimer(HWND ah_wnd, UINT id) {}
    //virtual HICON OnQUERYDRAGICON(HWND ah_wnd) {}
    virtual void OnMeasureItem(HWND ah_wnd, MEASUREITEMSTRUCT* lpMeasureItem) {}
    virtual void OnSysCommand(HWND ah_wnd, UINT cmd, int x, int y) {}
    virtual void OnVSCroll(HWND ah_wnd, HWND ah_wnd_ctrl, UINT code, int pos) {}
    virtual void OnHSCroll(HWND ah_wnd, HWND ah_wnd_ctrl, UINT code, int pos) {}
    virtual void OnSetFocus(HWND ah_wnd, HWND ah_wnd_old_focus) {}
    virtual void OnKillFocus(HWND ah_whd, HWND ah_wnd_new_focus) {}
    virtual void OnContextMenu(HWND ah_wnd, HWND ah_wnd_context, UINT x_pos, UINT y_pos) {}
    virtual void OnSettingChange(HWND ah_wnd, UINT flags, LPCTSTR lpszSection) {}
    virtual void OnKeyDown(HWND ah_wnd, UINT vk, BOOL down, int repeat, UINT flags) {}
    virtual void OnKeyUp(HWND ah_wnd, UINT vk, BOOL down, int repeat, UINT flags) {}
    virtual void OnMouseWheel(HWND ah_wnd, int x, int y, int delta, UINT keys) {}
    virtual void OnCtlColor(HWND ah_wnd, HDC hdc, HWND ah_wnd_child, int type) {}
    virtual void OnDrawItem(HWND ah_wnd, const DRAWITEMSTRUCT* lpDrawItem) {}
    virtual void OnWindowPosChanging(HWND ah_wnd, WINDOWPOS* lpwpos) {}
    virtual void OnActivate(HWND ah_wnd, UINT state, HWND ah_wnd_act_deact, BOOL minimized) {}
    virtual void OnChar(HWND ah_wnd, TCHAR ch, int repeat) {}

protected:
    virtual PCTSTR  ClassName() const = 0;
    virtual int CmdShow() const = 0;
    virtual INT_PTR HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;


};

class WinApp : public BaseWindow
{
public:
    WinApp(const wchar_t* ap_class_name, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
    virtual ~WinApp();

    void OnDestroy(HWND ah_wnd);
    void OnCommand(HWND ah_wnd, int a_id, HWND ah_wnd_ctrl, UINT codeNotify);
    void OnPaint(HWND ah_wnd);
    void OnLButtonDown(HWND ah_wnd, BOOL a_double_click, int a_x, int a_y, UINT a_key_flags);
    BOOL OnCreate(HWND ah_wnd, LPCREATESTRUCT lpCreateStruct);

    PCTSTR  ClassName() const { return m_class_name; }
    int CmdShow() const {return m_cmd_show;}
    INT_PTR HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


private:
    const wchar_t* m_class_name;
    int m_cmd_show;
    HWND m_button1;
    HWND m_edit1;
    PAINTSTRUCT ps;
};