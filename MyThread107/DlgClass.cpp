#include "framework.h"
#include "MyThread107.h"
#include "DlgClass.h"

DlgClass::DlgClass() : m_hWnd(NULL), m_hdc(NULL)
{
	ps = { 0, };
}

INT_PTR CALLBACK DlgClass::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
	}
	return (INT_PTR)FALSE;
}

void DlgClass::OnCommand(HWND hwnd, int id, HWND wnd_ctrl, UINT codeNotify)
{
	if (id >= IDC_BUTTON1 && id <= IDC_BUTTON3) {
		SetDlgItemInt(m_hWnd, IDC_MSG_EDIT, id - IDC_BUTTON1 + 1, 1);
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


	return TRUE;
}

