#ifndef WIN_API_MAIN_H_
#define WIN_API_MAIN_H_

#include "resource.h"

template<class DERIVED_TYPE>
class BaseWindow
{
public:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hWnd = hWnd;
        }
        else
        {
            pThis = (DERIVED_TYPE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    explicit BaseWindow() : m_hWnd(NULL) { }
    virtual ~BaseWindow() {}

    BOOL Create(
        PCTSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    )
    {
        WNDCLASS wc = { 0 };

        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.lpfnWndProc = DERIVED_TYPE::WndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();
        wc.lpszMenuName = NULL;
        wc.style = CS_HREDRAW | CS_VREDRAW;

        RegisterClass(&wc);

        m_hWnd = CreateWindowEx(
            dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
        );
        return (m_hWnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hWnd; }
    HINSTANCE m_hInst = NULL;

protected:
    virtual PCTSTR  ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    HWND m_hWnd = NULL;
};

class MainWindow : public BaseWindow<MainWindow>
{

public:
    explicit MainWindow();
    virtual ~MainWindow();

    void OnCreate();
    void OnPaint();
    void OnDestroy();
    void OnCommand(WPARAM wParam);
    PCTSTR  ClassName() const { return _T("Window Class"); }
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif