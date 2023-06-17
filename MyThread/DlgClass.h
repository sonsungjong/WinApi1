#pragma once
class DlgClass
{
public:
	DlgClass() : m_hWnd(NULL) {}
	~DlgClass() {}

	static LRESULT CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        DlgClass* dlg;
        if (msg == WM_INITDIALOG) {
            dlg = new DlgClass();
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(dlg));
            dlg->m_hWnd = hWnd;
        }
        else {
            dlg = (DlgClass*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        }

        if (dlg) {
            LRESULT result = dlg->HandleMessage(hWnd, msg, wParam, lParam);

            if (msg == WM_NCDESTROY) {
                delete dlg;
                SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            }

            return result;
        }
        else {
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }

	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCommand(int id);
	void OnPaint();
	void OnDestroy();
	void OnInitialDialog();

	HWND m_hWnd;

private:

};