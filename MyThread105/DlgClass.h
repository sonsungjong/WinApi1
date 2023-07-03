#ifndef DLG_CLASS_H_
#define DLG_CLASS_H_

class DlgClass
{
public:
	DlgClass();
	~DlgClass() {}

	static LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
			LRESULT result = dlg->HandleMessage(hwnd, msg, wParam, lParam);

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

	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCommand(HWND ah_wnd, int id, HWND ah_wnd_ctrl, UINT codeNotify);
	void OnPaint(HWND hwnd);
	void OnDestroy(HWND hwnd);
	void OnInitialDialog();

	HWND m_hWnd;
	HDC m_hdc;

private:
	PAINTSTRUCT ps;
};

#endif			DLG_CLASS_H_