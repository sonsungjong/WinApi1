#include "pch.h"
#include "framework.h"
#include "LoginWnd2.h"

HINSTANCE g_hInst;
HWND g_hWnd;

HWND g_hEdit1;
HWND g_hEdit2;
HWND g_hButton;

void CenterControls(HWND hWnd) {
	RECT rect;
	GetClientRect(hWnd, &rect);

	// 각 컨트롤의 크기와 위치 설정
	int controlWidth = 200;
	int controlHeight = 25;
	int spacing = 10;

	int xPos = (rect.right - controlWidth) / 2;
	int yPos = (rect.bottom - (controlHeight * 2 + spacing + controlHeight)) / 2;

	// 에디트 컨트롤과 버튼 위치 설정
	SetWindowPos(g_hEdit1, NULL, xPos, yPos, controlWidth, controlHeight, SWP_NOZORDER);
	SetWindowPos(g_hEdit2, NULL, xPos, yPos + controlHeight + spacing, controlWidth, controlHeight, SWP_NOZORDER);
	SetWindowPos(g_hButton, NULL, xPos, yPos + (controlHeight + spacing) * 2, controlWidth, controlHeight, SWP_NOZORDER);
}


INT_PTR CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	TCHAR my_class_name[] = _T("main_wnd_class");
	g_hInst = hInstance;

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInst;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszClassName = my_class_name;
	RegisterClassExW(&wcex);

	// 화면 중앙 위치 및 크기 계산
	int windowWidth = 1280;
	int windowHeight = 960;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int posX = (screenWidth - windowWidth) / 2;
	int posY = (screenHeight - windowHeight) / 2;

	g_hWnd = CreateWindow(my_class_name, _T("로그인 화면"), WS_POPUP, posX, posY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	// 모서리 둥글게 (WIN11)
	DWM_WINDOW_CORNER_PREFERENCE preference = DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_ROUND;
	DwmSetWindowAttribute(g_hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct) {
	g_hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL,
		WS_CHILD | WS_VISIBLE ,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);

	g_hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL,
		WS_CHILD | WS_VISIBLE ,
		0, 0, 0, 0, hWnd, NULL, g_hInst, NULL);

	g_hButton = CreateWindow(L"BUTTON", L"로그인",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0, hWnd, (HMENU)1, g_hInst, NULL);

	CenterControls(hWnd);

	return TRUE;
}

void OnPaint(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
	EndPaint(hWnd, &ps);
}

void OnDestroy(HWND hWnd) {
	PostQuitMessage(0);
}

void OnCommand(HWND ah_wnd, int id, HWND ah_wnd_ctrl, UINT codeNotify)
{
	if (id == IDCANCEL)
	{
		EndDialog(g_hWnd, id);
	}
	else if (id == 1) {
		MessageBox(g_hWnd, _T("id"), _T("ok"), MB_OK);
	}
}