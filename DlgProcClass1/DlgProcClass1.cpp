// DlgProcClass1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DlgProcClass1.h"

DlgClass* dlg;

LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        dlg->OnPaint();
    }
    return (LRESULT)TRUE;
    case WM_COMMAND:
    {
        dlg->OnCommand(LOWORD(wParam));
    }
    return (LRESULT)TRUE;
    case WM_INITDIALOG:
        dlg->m_hWnd = hWnd;
        dlg->OnInitialDialog();

        return (LRESULT)TRUE;
    case WM_DESTROY:
        dlg->OnDestroy();
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    }
    return (LRESULT)FALSE;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    dlg = new DlgClass();
    dlg->m_hInst = hInstance;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DlgProc);

    return 0;
}

DlgClass::DlgClass()
{
    m_hInst = 0;
    m_hWnd = 0;
}

DlgClass::~DlgClass()
{
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
    if(id == IDCANCEL)
    {
        EndDialog(m_hWnd, id);
    }
    else if (id == IDOK) {
        MessageBox(m_hWnd, _T("id"), _T("ok"), MB_OK);
    }
}

void DlgClass::OnInitialDialog()
{
    SetWindowPos(m_hWnd, HWND_TOP, 100, 100, 475, 500, NULL);
}