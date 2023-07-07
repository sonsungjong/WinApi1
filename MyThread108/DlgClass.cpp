#include "framework.h"
#include "MyThread108.h"
#include "DlgClass.h"

DlgClass::DlgClass() : m_hWnd(NULL), m_hdc(NULL), 
	m_font_edit(NULL), m_font_button(NULL)
{
	ps = { 0, };

	m_reset_flag = 0;
	m_operation_flag = 0;
	m_first_num = 0;
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

BOOL DlgClass::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	// 폰트 생성 후 컨트롤에 적용

	// 초기값 0 셋팅
	SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, 0, TRUE);

	return TRUE;
}

void DlgClass::PushNumber(int id)
{

}

void DlgClass::PushClear()
{
	SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, 0, TRUE);
	m_reset_flag = 0;
	m_operation_flag = 0;
	m_first_num = 0;
}

void DlgClass::PushBack()
{

}

void DlgClass::PushOperation(int id)
{

}

void DlgClass::OnCommand(HWND hwnd, int id, HWND wnd_ctrl, UINT codeNotify)
{
	if (id >= IDC_BUTTON0 && id <= IDC_BUTTON9) {
		PushNumber(id);
	}
	else if (id == IDC_CLEAR_BTN) {
		PushClear();
	}
	else if (id == IDC_BACK_BTN) {
		PushBack();
	}
	else if (id >= IDC_ADD_BTN && id <= IDC_DIV_BTN) {
		PushOperation(id);
	}
}

void DlgClass::OnPaint(HWND hwnd)
{
	m_hdc = BeginPaint(hwnd, &ps);

	EndPaint(hwnd, &ps);
}

void DlgClass::OnDestroy(HWND hwnd)
{
	// 폰트가 NULL이 아니면 제거

	PostQuitMessage(0);
}

void DlgClass::OnClose(HWND hwnd)
{
	DestroyWindow(m_hWnd);
}

