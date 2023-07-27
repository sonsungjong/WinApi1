#ifndef DLG_CLASS_H_
#define DLG_CLASS_H_

class DlgClass
{
public:
	DlgClass();
	virtual ~DlgClass();

	static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		DlgClass* dlg;
		if (msg == WM_INITDIALOG) {
			dlg = new DlgClass();
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(dlg));
			dlg->m_hWnd = hwnd;
		}
		else {
			dlg = (DlgClass*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		if (dlg) {
			INT_PTR result = dlg->HandleMessage(hwnd, msg, wParam, lParam);

			if (msg == WM_NCDESTROY) {
				delete dlg;
				SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
			}
			return result;
		}
		else {
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	INT_PTR CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCommand(HWND hwnd, int id, HWND wnd_ctrl, UINT codeNotify);
	void OnPaint(HWND hwnd);
	void OnDestroy(HWND hwnd);
	void OnClose(HWND hwnd);
	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	BOOL OnNotify(int ctrl_id, NMHDR* pnmh);
	void OnLButtonDown(HWND hwnd, BOOL a_double_click, int x, int y, UINT a_key_flags);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT a_key_flags);
	void OnMouseMove(HWND hwnd, int x, int y, UINT a_key_flags);

	HWND m_hWnd;
	HDC m_hdc;

protected:
	PAINTSTRUCT ps;
	RECT m_wnd_size;

	RECT m_rect;
	HRGN m_star_rgn;
	HRGN m_ellipse_rgn;

	char m_click_flag;
	POINT m_prev_point;
	HBRUSH m_border_brush;
};

#endif			DLG_CLASS_H_