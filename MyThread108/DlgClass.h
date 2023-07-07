#ifndef DLG_CLASS_H_
#define DLG_CLASS_H_

class DlgClass
{
public:
	DlgClass();
	virtual ~DlgClass() {}

	static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		DlgClass* dlg;
		if (msg == WM_INITDIALOG) {
			dlg = new DlgClass();
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(dlg));
			dlg->m_hWnd = hwnd;
		}else{
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

	void PushNumber(int id);
	void PushClear();
	void PushBack();
	void PushOperation(int id);


	HWND m_hWnd;
	HDC m_hdc;

private:
	PAINTSTRUCT ps;

	HFONT m_font_edit;
	HFONT m_font_button;
	char m_reset_flag;
	unsigned char m_operation_flag;
	int m_first_num;
};

#endif		DLG_CLASS_H_