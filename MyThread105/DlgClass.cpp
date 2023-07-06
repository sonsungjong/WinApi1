#include "framework.h"
#include "MyThread105.h"
#include "DlgClass.h"

DlgClass::DlgClass() : m_hWnd(NULL), m_hdc(NULL)
{
	ps = { 0, };
}

INT_PTR CALLBACK DlgClass::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
		m_hdc = BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
		break;
	case WM_INITDIALOG:
		m_hWnd = hwnd;
		break;

	case WM_COMMAND:
		OnCommand(m_hWnd, LOWORD(wParam), NULL, NULL);
		break;
	case WM_DESTROY:
		OnDestroy(m_hWnd);
		break;
	}
	return (INT_PTR)FALSE;
}

void DlgClass::OnCommand(HWND ah_wnd, int id, HWND ah_wnd_ctrl, UINT codeNotify)
{
	if (id == IDCANCEL) {
		EndDialog(m_hWnd, id);
	}
	else if (id == IDC_READ_BTN) {
		HWND h_edit = GetDlgItem(m_hWnd, IDC_MSG_EDIT);
		int len = SendMessage(h_edit, WM_GETTEXTLENGTH, NULL, NULL);

		if (len > 9) {
			MessageBox(m_hWnd, _T("글자수 10 이상"), _T("제목"), MB_OK);
		}
		else {
			TCHAR str[10] = { 0, };
			int str_len = GetDlgItemText(m_hWnd, IDC_MSG_EDIT, str, 10);
			MessageBox(m_hWnd, str, _T("제목"), MB_OK);
		}

	}
	else if (id == IDC_WRITE_BTN) {
		const TCHAR* str = _T("에디트 컨트롤에 글 쓰기");
		SetDlgItemText(m_hWnd, IDC_MSG_EDIT, str);
	}
}

void DlgClass::OnPaint(HWND hwnd)
{

}

void DlgClass::OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

void DlgClass::OnInitialDialog()
{

}