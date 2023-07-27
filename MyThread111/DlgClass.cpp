#include "framework.h"
#include "MyThread111.h"
#include "DlgClass.h"

DlgClass::DlgClass()
	: m_hWnd(NULL), m_hdc(NULL)
{
	ps = { 0, };
	m_prev_point = { 0, };
	m_wnd_size = { 0, };

	m_rect.left = 10;
	m_rect.top = 10;
	m_rect.right = 110;
	m_rect.bottom = 110;

	POINT star_point[11] = {
		{101, 0}, {77,77}, {0,77}, {62,125}, {39,201}, {101,153}, {163,201}, {138,125}, {201,77}, {124,77}, {101,0}
	};

	m_star_rgn = CreatePolygonRgn(star_point, 11, ALTERNATE);
	m_ellipse_rgn = CreateEllipticRgn(210, 10, 300, 100);
	m_border_brush = CreateSolidBrush(RGB(0, 0, 0));

	m_click_flag = 0;
}

DlgClass::~DlgClass() {}

INT_PTR CALLBACK DlgClass::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hwnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hwnd, WM_MOUSEMOVE, OnMouseMove);
		HANDLE_MSG(hwnd, WM_LBUTTONUP, OnLButtonUp);
	case WM_NOTIFY:
		return (BOOL)OnNotify((int)wParam, (NMHDR*)lParam);
	}
	return (INT_PTR)FALSE;
}

void DlgClass::OnCommand(HWND hwnd, int id, HWND wnd_ctrl, UINT codeNotify)
{
	if (IDOK == id) {
		MessageBox(hwnd, _T("msg"), _T("caption"), MB_OK);
	}
}
void DlgClass::OnPaint(HWND hwnd)
{
	m_hdc = BeginPaint(hwnd, &ps);

	PaintRgn(m_hdc, m_ellipse_rgn);
	FrameRgn(m_hdc, m_ellipse_rgn, m_border_brush, 1, 1);

	PaintRgn(m_hdc, m_star_rgn);
	FrameRgn(m_hdc, m_star_rgn, m_border_brush, 5, 5);

	Rectangle(m_hdc, m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);

	EndPaint(hwnd, &ps);
}
void DlgClass::OnDestroy(HWND hwnd)
{

}
void DlgClass::OnClose(HWND hwnd)
{
	DestroyWindow(m_hWnd);
}
BOOL DlgClass::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	GetWindowRect(hwnd, &m_wnd_size);

	return TRUE;
}
BOOL DlgClass::OnNotify(int ctrl_id, NMHDR* pnmh)
{

	return TRUE;
}

void DlgClass::OnLButtonDown(HWND hwnd, BOOL a_double_click, int x, int y, UINT a_key_flags)
{
	if (x > m_rect.left && x < m_rect.right && y > m_rect.top && y < m_rect.bottom) {
		m_click_flag = 1;
		m_prev_point.x = x;
		m_prev_point.y = y;
		SetCapture(hwnd);
	}
	else if (PtInRegion(m_star_rgn, x, y)) {
		m_click_flag = 2;
		m_prev_point.x = x;
		m_prev_point.y = y;
		SetCapture(hwnd);
	}
	else if (PtInRegion(m_ellipse_rgn, x, y)) {
		m_click_flag = 3;
		m_prev_point.x = x;
		m_prev_point.y = y;
		SetCapture(hwnd);
	}
}

void DlgClass::OnMouseMove(HWND hwnd, int x, int y, UINT a_key_flags)
{
	if (m_click_flag == 1) {
		LONG move_distance_x = x - m_prev_point.x;
		LONG move_distance_y = y - m_prev_point.y;

		m_rect.left += move_distance_x;
		m_rect.right += move_distance_x;
		m_rect.top += move_distance_y;
		m_rect.bottom += move_distance_y;

		InvalidateRect(hwnd, &m_wnd_size, 1);
		m_prev_point = { x, y };
	}
	else if (m_click_flag == 2) {
		LONG move_distance_x = x - m_prev_point.x;
		LONG move_distance_y = y - m_prev_point.y;

		OffsetRgn(m_star_rgn, move_distance_x, move_distance_y);

		InvalidateRect(hwnd, &m_wnd_size, 1);
		m_prev_point = { x, y };
	}
	else if (m_click_flag == 3) {
		LONG move_distance_x = x - m_prev_point.x;
		LONG move_distance_y = y - m_prev_point.y;

		OffsetRgn(m_ellipse_rgn, move_distance_x, move_distance_y);

		InvalidateRect(hwnd, &m_wnd_size, 1);
		m_prev_point = { x, y };
	}
}

void DlgClass::OnLButtonUp(HWND hwnd, int x, int y, UINT a_key_flags)
{
	if (m_click_flag != 0) {
		m_click_flag = 0;
		ReleaseCapture();
	}
}
