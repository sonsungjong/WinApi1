#include "framework.h"
#include "MyThread106.h"
#include "DlgClass.h"

DlgClass::DlgClass() : m_hWnd(NULL), m_hdc(NULL), m_data_list(NULL)
{
	ps = { 0, };
}

LRESULT CALLBACK DlgClass::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
	}
	return (LRESULT)FALSE;
}

void DlgClass::OnCommand(HWND hwnd, int id, HWND wnd_ctrl, UINT codeNotify)
{
	if (id == IDC_ADD_BTN) {
		ClickAddBtn();
	}
}
void DlgClass::OnPaint(HWND hwnd)
{
	m_hdc = BeginPaint(hwnd, &ps);

	EndPaint(hwnd, &ps);
}
void DlgClass::OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}
void DlgClass::OnClose(HWND hwnd)
{
	DestroyWindow(m_hWnd);
}
BOOL DlgClass::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	m_data_list = GetDlgItem(m_hWnd, IDC_DATA_LIST);

	return TRUE;
}

void DlgClass::ClickAddBtn()
{
	TCHAR str[128] = { 0, };
	GetDlgItemText(m_hWnd, IDC_MSG_EDIT, str, 128);

	// SendMessage(m_data_list, LB_ADDSTRING, 0, (LPARAM)str);
	int index = (int)SendMessage(m_data_list, LB_INSERTSTRING, -1, (LPARAM)str);
	SendMessage(m_data_list, LB_SETCURSEL, index, NULL);

	SetDlgItemText(m_hWnd, IDC_MSG_EDIT, _T(""));
}
