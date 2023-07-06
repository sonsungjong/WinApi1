// WinDCP.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include <windowsx.h>
#include "DCP.h"

#include "WinDCP.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	MainWindow win;
	win.m_hInst = hInstance;

	if (!win.Create(L"WinApi1", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}
	// ToDo : ÃÊ±âÈ­ ÀÛ¾÷

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


// »ý¼ºÀÚ
MainWindow::MainWindow()
{
}

// ¼Ò¸êÀÚ
MainWindow::~MainWindow()
{
}

INT_PTR MainWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(m_hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(m_hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(m_hWnd, WM_DESTROY, OnDestroy);
	case WM_COMMAND:
		OnCommand(wParam);
		break;
	}

	return DefWindowProc(m_hWnd, msg, wParam, lParam);
}

BOOL MainWindow::OnCreate(HWND ah_wnd, LPCREATESTRUCT lpCreateStruct)
{
	RECT r;
	GetClientRect(m_hWnd, &r);

	m_dcp.CreateDCP(r.right - r.left, r.bottom - r.top);
	m_dcp.DCPTextSetting(_T("¸¼Àº °íµñ"), 24, RGB24(0, 150, 150));

	return TRUE;
}

void MainWindow::OnPaint(HWND ah_wnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps);

	m_dcp.Clear(RGB24(50, 70, 90));
	m_dcp.FillSolidEllipse(100, 100, 200, 200, RGB24(0, 255, 0));
	m_dcp.FillSolidEllipse(150, 150, 250, 250);
	
	m_dcp.DCPText(0, 0, _T("¾È³çÇÏ¼¼¿À"), 5);
	m_dcp.DCPText(0, 100, _T("hello world!"), 12);
	
	m_dcp.Draw(hdc);
	EndPaint(m_hWnd, &ps);
}



void MainWindow::OnDestroy(HWND ah_wnd)
{
	PostQuitMessage(0);
}

void MainWindow::OnCommand(WPARAM wParam)
{
	int wm_id = LOWORD(wParam);
}
