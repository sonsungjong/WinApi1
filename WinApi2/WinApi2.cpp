#include "framework.h"
#include "WinApi2.h"

class WinApp {
public:
    WinApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
    virtual ~WinApp() {}
    int run();
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    BOOL OnCreate(HWND ah_wnd, LPCREATESTRUCT lpCreateStruct) {
        
    }
    void OnDestroy(HWND ah_wnd) {
        PostQuitMessage(0);
    }

private:
    LRESULT CALLBACK RunWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HINSTANCE m_hInst;
    HINSTANCE hPrevInstance;
    LPWSTR lpCmdLine;
    int nCmdShow;

    HWND m_hWnd;
};

// »ý¼ºÀÚ
WinApp::WinApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
    : m_hInst(hInstance), hPrevInstance(hPrevInstance), lpCmdLine(lpCmdLine), nCmdShow(nCmdShow), m_hWnd(NULL) {

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"SampleWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    RegisterClassEx(&wcex);

    hWnd = CreateWindowExW();
}

LRESULT CALLBACK WinApp::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WinApp* app;
    if (msg == WM_NCCREATE) {
        app = (WinApp*)(((CREATESTRUCT*)lParam)->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(app));
        app->m_hWnd = hWnd;
    }
    else {
        app = (WinApp*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (app) {
        return app->RunWndProc(hWnd, msg, wParam, lParam);
    }
    else {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

LRESULT WinApp::RunWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WinApp::run() {
    // Normal WinMain code here...
    // Instead of calling WinMain, you would instantiate the WinApp class and call its run method.
    return 0;  // Return exit code.
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    WinApp app(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    return app.run();
}