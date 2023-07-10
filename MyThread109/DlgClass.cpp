#include "framework.h"
#include <CommCtrl.h>
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
	case WM_NOTIFY:
		return (BOOL)OnNotify((int)wParam, (NMHDR*)lParam);
	}
	return (INT_PTR)FALSE;
}

void DlgClass::OnCommand(HWND hwnd, int id, HWND wnd_ctrl, UINT codeNotify)
{
	if (id == IDC_ADD_BTN) {
		PushAddBtn();
	}
	else if (id == IDC_MODIFY_BTN) {
		PushModifyBtn();
	}
	else if (id == IDOK) {
		MessageBox(m_hWnd, _T("Ȯ�� ��ư"), _T("����"), MB_OK);
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
	// ��Ʈ�� �ּҸ� ������ �����Ѵ�
	m_list_ctrl = GetDlgItem(m_hWnd, IDC_DATA_LIST);

	// ����Ʈ ��Ʈ�� �Ӽ��� �����Ѵ�
	DWORD style = GetWindowLongPtr(m_list_ctrl, GWL_STYLE);
	style |= LVS_SINGLESEL;
	style |= LVS_SHOWSELALWAYS;
	style &= ~LVS_TYPEMASK;				// ���� ��Ÿ�� ����
	style |= LVS_REPORT;					// Report ��Ÿ�� ����
	SetWindowLongPtr(m_list_ctrl, GWL_STYLE, style);
	RedrawWindow(m_list_ctrl, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);

	// ����Ʈ ��Ʈ�� �÷����� �����Ѵ�
	LV_COLUMN lv_column;
	lv_column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	int column = 0;

	lv_column.fmt = LVCFMT_CENTER;
	lv_column.cx = 80;
	lv_column.pszText = (TCHAR*)_T("     �̸�");
	SendMessage(m_list_ctrl, LVM_INSERTCOLUMN, (WPARAM)column++, (LPARAM)(&lv_column));

	lv_column.cx = 100;
	lv_column.pszText = (TCHAR*)_T("��ȭ��ȣ");
	SendMessage(m_list_ctrl, LVM_INSERTCOLUMN, (WPARAM)column++, (LPARAM)(&lv_column));

	lv_column.cx = 200;
	lv_column.pszText = (TCHAR*)_T("�ּ�");
	SendMessage(m_list_ctrl, LVM_INSERTCOLUMN, (WPARAM)column++, (LPARAM)(&lv_column));

	// ����Ʈ ��Ʈ�� Ȯ�� ������ ���ش� (���� | ���ټ���)
	SendMessage(m_list_ctrl, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)NULL, (LPARAM)(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT));

	// �׽�Ʈ �Է�
	SetDlgItemText(m_hWnd, IDC_NAME_EDIT, _T("ȫ�浿"));
	SetDlgItemText(m_hWnd, IDC_PHONE_EDIT, _T("010-1111-1111"));
	SetDlgItemText(m_hWnd, IDC_ADDRESS_EDIT, _T("���� ����"));
	PushAddBtn();

	SetDlgItemText(m_hWnd, IDC_NAME_EDIT, _T("������"));
	SetDlgItemText(m_hWnd, IDC_PHONE_EDIT, _T("010-2222-2222"));
	SetDlgItemText(m_hWnd, IDC_ADDRESS_EDIT, _T("��õ ����"));
	PushAddBtn();

	SetDlgItemText(m_hWnd, IDC_NAME_EDIT, _T("�̼���"));
	SetDlgItemText(m_hWnd, IDC_PHONE_EDIT, _T("010-3333-3333"));
	SetDlgItemText(m_hWnd, IDC_ADDRESS_EDIT, _T("��� �Ȼ�"));
	PushAddBtn();

	return TRUE;
}

BOOL DlgClass::OnNotify(int ctrl_id, NMHDR* pnmh)
{
	if (pnmh->idFrom == IDC_DATA_LIST && pnmh->code == LVN_ITEMCHANGED)
	{
		LPNMLISTVIEW pnmlv = (LPNMLISTVIEW)(pnmh);

		//if((pnmlv->uChanged & LVIF_STATE) && (pnmlv->uNewState == LVIS_SELECTED || pnmlv->uOldState == LVIS_SELECTED))
		if (pnmlv->uNewState & LVIS_SELECTED)
		{
			TCHAR name[64] = { 0, };
			TCHAR phone[32] = { 0, };
			TCHAR address[256] = { 0, };
			int row_index = pnmlv->iItem;
			int column_index = 0;

			ListView_GetItemText(m_list_ctrl, row_index, column_index++, name, 64);
			ListView_GetItemText(m_list_ctrl, row_index, column_index++, phone, 32);
			ListView_GetItemText(m_list_ctrl, row_index, column_index++, address, 256);

			SetDlgItemText(m_hWnd, IDC_NAME_EDIT, name);
			SetDlgItemText(m_hWnd, IDC_PHONE_EDIT, phone);
			SetDlgItemText(m_hWnd, IDC_ADDRESS_EDIT, address);
		}
	}

	return TRUE;
}

void DlgClass::PushAddBtn()
{
	LV_ITEM lv_item;
	lv_item.mask = LVIF_TEXT;

	TCHAR name[64] = { 0, };
	TCHAR phone[32] = { 0, };
	TCHAR address[256] = { 0, };

	GetDlgItemText(m_hWnd, IDC_NAME_EDIT, name, 64);
	GetDlgItemText(m_hWnd, IDC_PHONE_EDIT, phone, 32);
	GetDlgItemText(m_hWnd, IDC_ADDRESS_EDIT, address, 256);

	int row_index = SendMessage(m_list_ctrl, LVM_GETITEMCOUNT, NULL, NULL);
	int column_index = 0;

	lv_item.iItem = row_index;
	lv_item.iSubItem = column_index++;
	lv_item.pszText = name;
	SendMessage(m_list_ctrl, LVM_INSERTITEM, (WPARAM)NULL, (LPARAM)&lv_item);

	lv_item.iSubItem = column_index++;
	lv_item.pszText = phone;
	SendMessage(m_list_ctrl, LVM_SETITEM, (WPARAM)NULL, (LPARAM)&lv_item);

	lv_item.iSubItem = column_index++;
	lv_item.pszText = address;
	SendMessage(m_list_ctrl, LVM_SETITEM, (WPARAM)NULL, (LPARAM)&lv_item);

	SetDlgItemText(m_hWnd, IDC_NAME_EDIT, _T(""));
	SetDlgItemText(m_hWnd, IDC_PHONE_EDIT, _T(""));
	SetDlgItemText(m_hWnd, IDC_ADDRESS_EDIT, _T(""));
}

void DlgClass::PushModifyBtn()
{
	TCHAR name[64] = { 0, };
	TCHAR phone[32] = { 0, };
	TCHAR address[256] = { 0, };

	GetDlgItemText(m_hWnd, IDC_NAME_EDIT, name, 64);
	GetDlgItemText(m_hWnd, IDC_PHONE_EDIT, phone, 32);
	GetDlgItemText(m_hWnd, IDC_ADDRESS_EDIT, address, 256);

	int row_index = SendMessage(m_list_ctrl, LVM_GETNEXTITEM, (WPARAM)-1, MAKELPARAM(LVNI_SELECTED, 0));
	int column_index = 0;

	LVITEM lv_item;
	lv_item.mask = LVIF_TEXT;
	lv_item.iItem = row_index;
	lv_item.iSubItem = column_index++;
	lv_item.pszText = name;
	SendMessage(m_list_ctrl, LVM_SETITEM, (WPARAM)NULL, (LPARAM)&lv_item);

	lv_item.iSubItem = column_index++;
	lv_item.pszText = phone;
	SendMessage(m_list_ctrl, LVM_SETITEM, (WPARAM)NULL, (LPARAM)&lv_item);

	lv_item.iSubItem = column_index++;
	lv_item.pszText = address;
	SendMessage(m_list_ctrl, LVM_SETITEM, (WPARAM)NULL, (LPARAM)&lv_item);
}

