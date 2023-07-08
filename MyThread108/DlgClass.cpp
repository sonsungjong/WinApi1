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
	m_font_edit = CreateFont(36, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));
	HWND edit = GetDlgItem(m_hWnd, IDC_RESULT_EDIT);
	SendMessage(edit, WM_SETFONT, (WPARAM)m_font_edit, (LPARAM)TRUE);

	m_font_button = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("맑은 고딕"));
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON0), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON1), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON2), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON3), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON4), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON5), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON6), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON7), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON8), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BUTTON9), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_CLEAR_BTN), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_BACK_BTN), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_EQUAL_BTN), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_ADD_BTN), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_SUB_BTN), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_MUL_BTN), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);
	SendMessage(GetDlgItem(m_hWnd, IDC_DIV_BTN), WM_SETFONT, (WPARAM)m_font_button, (LPARAM)TRUE);


	// 초기값 0 셋팅
	SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, 0, TRUE);

	return TRUE;
}

void DlgClass::PushNumber(int id)
{
	int prev_num = GetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, NULL, TRUE);
	long long result_num = 0;
	
	if (m_reset_flag == 1) {
		SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, id - IDC_BUTTON0, TRUE);
		m_reset_flag = 0;
	}
	else {
		if (prev_num >= 0) {
			result_num = ((long long)prev_num * 10) + (id - IDC_BUTTON0);
		}
		else {
			result_num = ((long long)prev_num * 10) - (id - IDC_BUTTON0);
		}

		if (result_num >= INT_MIN && result_num <= INT_MAX) {
			SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, (int)result_num, TRUE);
		}
	}
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
	int prev_num = GetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, NULL, TRUE);
	SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, prev_num / 10, TRUE);
}

void DlgClass::PushOperation(int id)
{
	m_reset_flag = 1;
	m_first_num = GetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, NULL, TRUE);
	m_operation_flag = id - IDC_ADD_BTN + 1;
}

void DlgClass::PushEqual()
{
	int second_num = GetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, NULL, TRUE);

	if (m_operation_flag == 1) {
		SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, m_first_num + second_num, TRUE);
	}
	else if (m_operation_flag == 2) {
		SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, m_first_num - second_num, TRUE);
	}
	else if (m_operation_flag == 3) {
		SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, m_first_num * second_num, TRUE);
	}
	else if (m_operation_flag == 4) {
		if (second_num != 0) {
			SetDlgItemInt(m_hWnd, IDC_RESULT_EDIT, m_first_num / second_num, TRUE);
		}
	}
	m_reset_flag = 1;
	m_operation_flag = 0;
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
	else if (id == IDC_EQUAL_BTN) {
		PushEqual();
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
	if (m_font_button != NULL) {
		DeleteObject(m_font_button);
	}

	if (m_font_edit != NULL) {
		DeleteObject(m_font_edit);
	}

	PostQuitMessage(0);
}

void DlgClass::OnClose(HWND hwnd)
{
	DestroyWindow(m_hWnd);
}

