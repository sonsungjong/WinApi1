#include "framework.h"
#include "DlgClass.h"

INT_PTR DlgClass::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        OnPaint();
    }
    return (INT_PTR)TRUE;
    case WM_COMMAND:
    {
        OnCommand(LOWORD(wParam));
    }
    return (INT_PTR)TRUE;
    case WM_INITDIALOG:
        OnInitialDialog();
        return (INT_PTR)TRUE;
    case WM_DESTROY:
        OnDestroy();
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    }
    return (INT_PTR)FALSE;
}

void DlgClass::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hWnd, &ps);
    // TODO: Add any drawing code that uses hdc here...
    Rectangle(hdc, 10, 10, 120, 120);

    EndPaint(m_hWnd, &ps);
}

void DlgClass::OnDestroy()
{
    PostQuitMessage(0);
}

void DlgClass::OnCommand(int id)
{
    if (id == IDCANCEL)
    {
        EndDialog(m_hWnd, id);
    }
    else if (id == IDOK) {
        MessageBox(m_hWnd, _T("id"), _T("ok"), MB_OK);
    }
}

void DlgClass::OnInitialDialog()
{
    SetWindowPos(m_hWnd, HWND_TOP, 100, 100, 550, 500, NULL);
}
