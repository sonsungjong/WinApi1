#include "framework.h"
#include "DlgClass.h"
#include "MyThread103.h"

LRESULT DlgClass::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
    
    case WM_INITDIALOG:
        OnInitialDialog();
        return (LRESULT)TRUE;

    }
    return (LRESULT)FALSE;
}

void DlgClass::OnCommand(HWND ah_wnd, int id, HWND ah_wnd_ctrl, UINT codeNotify)
{
    if (id == IDCANCEL)
    {
        EndDialog(m_hWnd, id);
    }
    else if (id == IDC_BUTTON1) {
        MessageBox(m_hWnd, _T("id"), _T("ok"), MB_OK);
    }
}

void DlgClass::OnPaint(HWND hwnd)
{
    hdc = BeginPaint(m_hWnd, &ps);
    // TODO: Add any drawing code that uses hdc here...
    Rectangle(hdc, 10, 10, 120, 120);

    EndPaint(m_hWnd, &ps);
}


void DlgClass::OnLButtonDown(HWND ah_wnd, BOOL a_double_click, int a_x, int a_y, UINT a_key_flags)
{
    hdc = GetDC(m_hWnd);
    if (a_key_flags & MK_SHIFT) {
        Rectangle(hdc, a_x - 30, a_y - 30, a_x + 30, a_y + 30);
    }
    else if (a_key_flags & MK_CONTROL) {
        Ellipse(hdc, a_x - 30, a_y - 30, a_x + 30, a_y + 30);
    }
    else {
        RoundRect(hdc, a_x - 30, a_y - 30, a_x + 30, a_y + 30, 10, 10);
        // °¡·Î µÕ±Û°Ô 10, ¼¼·Î µÕ±Û°Ô 10
    }
    ReleaseDC(m_hWnd, hdc);
}

void DlgClass::OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}



void DlgClass::OnInitialDialog()
{
    SetWindowPos(m_hWnd, HWND_TOP, 5, 5, 550, 500, NULL);
}