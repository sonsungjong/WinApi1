#pragma once
class DlgClass
{
public:
    DlgClass() : m_hWnd(NULL) {}
    ~DlgClass() {}

    static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        DlgClass* dlg;
        if (msg == WM_INITDIALOG) {
            dlg = new DlgClass();
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(dlg));
            dlg->m_hWnd = hWnd;
        }
        else {
            dlg = (DlgClass*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        }

        if (dlg) {
            INT_PTR result = dlg->HandleMessage(hWnd, msg, wParam, lParam);

            if (msg == WM_NCDESTROY) {
                delete dlg;
                SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            }

            return result;
        }
        else {
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }

    INT_PTR CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void OnCommand(HWND ah_wnd, int id, HWND ah_wnd_ctrl, UINT codeNotify);
    void OnPaint(HWND hwnd);
    void OnDestroy(HWND hwnd);
    void OnInitialDialog();
    void OnLButtonDown(HWND ah_wnd, BOOL a_double_click, int a_x, int a_y, UINT a_key_flags);
    void OnLButtonUp(HWND ah_wnd, BOOL a_double_click, int x, int y, UINT a_key_flags) {}
    void OnMouseMove(HWND ah_wnd, int x, int y, UINT a_key_flags) {}

    HWND m_hWnd;
    PAINTSTRUCT ps;
    HDC hdc;

private:

};