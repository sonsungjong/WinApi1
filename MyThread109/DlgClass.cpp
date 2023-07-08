#include "framework.h"
#include "MyThread109.h"
#include "DlgClass.h"

DlgClass::DlgClass()
	: m_hWnd(NULL), m_hdc(NULL)
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
	if (id == IDC_ADD_BTN) {
		MessageBox(m_hWnd, _T("추가 버튼"), _T("제목"), MB_OK);
	}
	else if (id == IDC_MODIFY_BTN) {
		MessageBox(m_hWnd, _T("수정 버튼"), _T("제목"), MB_OK);
	}
	else if (id == IDOK) {
		MessageBox(m_hWnd, _T("확인"), _T("제목"), MB_OK);
	}
	else if (id == IDCANCEL) {
		EndDialog(m_hWnd, FALSE);
	}
}

void DlgClass::OnPaint(HWND hwnd)
{
	m_hdc = BeginPaint(m_hWnd, &ps);

	EndPaint(m_hWnd, &ps);
}

void DlgClass::OnDestroy(HWND hwnd)
{
	// TODO

	PostQuitMessage(0);
}

void DlgClass::OnClose(HWND hwnd)
{
	DestroyWindow(m_hWnd);
}

BOOL DlgClass::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	// TODO

	return TRUE;
}