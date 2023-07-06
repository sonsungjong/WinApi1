#include "framework.h"
#include <windowsx.h>

#include "WinApi1.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	MainWindow win;
	win.m_hInst = hInstance;

	if (!win.Create(L"WinApi1", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}
	// ToDo : 초기화 작업


	ShowWindow(win.Window(), nCmdShow);
	UpdateWindow(win.Window());

	MSG msg = {};
	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int)msg.wParam;
}


// 생성자
MainWindow::MainWindow()
{
}

// 소멸자
MainWindow::~MainWindow()
{
}

INT_PTR MainWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		OnCreate();
		break;
	case WM_PAINT:
		OnPaint();
		break;
	case WM_DESTROY:
		OnDestroy();
		break;
	case WM_COMMAND:
		OnCommand(wParam);
		break;
	}

	return DefWindowProc(m_hWnd, msg, wParam, lParam);
}

void MainWindow::OnCreate()
{
}

void MainWindow::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps);
	FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
	EndPaint(m_hWnd, &ps);
}



void MainWindow::OnDestroy()
{
	PostQuitMessage(0);
}

void MainWindow::OnCommand(WPARAM wParam)
{
	int wm_id = LOWORD(wParam);
}
