// DlgProcClass1.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DlgProcClass1.h"
HINSTANCE hInst;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgClass::DlgProc);

    return 0;
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

INT_PTR DlgClass::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static DlgClass* dlg;
	if (msg == WM_INITDIALOG) {
		dlg = new DlgClass();
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(dlg));
		dlg->m_hWnd = hWnd;
	}
	else {
		(DlgClass*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (dlg) {
        INT_PTR result = dlg->HandleMessage(hWnd, msg, wParam, lParam);

		if (msg == WM_DESTROY) {
			delete dlg;
			dlg = NULL;
		}
	}
	else {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

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
