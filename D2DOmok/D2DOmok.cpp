#include "framework.h"
#include "D2DOmok.h"

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

using namespace D2D1;

// 가로줄 19, 세로줄 19
#define X_COUNT			19
#define Y_COUNT			19

// 최초 여백
#define START_X			50
#define START_Y			50

// 줄 간격
#define INTERVAL			26

// 바둑돌 반지름 (간격의 절반)
#define HALF_INTERVAL			INTERVAL/2

// 바둑돌 위치 보정 (클릭위치 보정)
#define XPOS(x)				(float)(START_X + (x)*INTERVAL)
#define YPOS(y)					(float)(START_Y + (y)*INTERVAL)

// 바둑돌이 있는지 여부
unsigned char g_dol[Y_COUNT][X_COUNT];

// 검은돌, 흰돌 순서 (0:검은돌 차례, 1:흰돌 차례)
unsigned char g_step;

// D2D 객체 생성 팩토리
ID2D1Factory* gp_factory;

// D2D 클라이언트에 그림을 그리는 객체
ID2D1HwndRenderTarget* gp_render_target;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	// D2D 라이브러리 초기화
	(void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	// D2D factory 생성
	if (S_OK != D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &gp_factory)) { 
		return 0; }

	TCHAR my_class_name[] = _T("wnd_class");
	WNDCLASS wc;

	wc.cbClsExtra = NULL;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = my_class_name;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	HWND h_wnd = CreateWindow(my_class_name, _T("Direct2D로 오목 만들기"), WS_OVERLAPPEDWINDOW, 50, 50, 600, 600, NULL, NULL, hInstance, NULL);
	ShowWindow(h_wnd, SW_SHOW);
	UpdateWindow(h_wnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// factory 제거
	gp_factory->Release();
	// 라이브러리 해제
	CoUninitialize();
	return (int)msg.wParam;
}

void OnPaint(HWND ah_wnd)
{
	// D2D 그리기 시작 (검정 배경)
	gp_render_target->BeginDraw();
	gp_render_target->Clear(ColorF(0.95686f, 0.69f, 0.3019f));				// 배경색

	// D2D Brush 생성
	ID2D1SolidColorBrush* p_line_brush, * p_black_brush, * p_white_brush;
	gp_render_target->CreateSolidColorBrush(ColorF(0.095686f, 0.069f, 0.03019f), &p_line_brush);
	gp_render_target->CreateSolidColorBrush(ColorF(0.0f, 0.0f, 0.0f), &p_black_brush);
	gp_render_target->CreateSolidColorBrush(ColorF(1.0f, 1.0f, 1.0f), &p_white_brush);

	D2D1_POINT_2F start_pos, end_pos;

	// 선 그리기
	// 수직으로 19개의 선을 그린다
	for (int x = 0; x < X_COUNT; x++) {
		start_pos.x = XPOS(x);
		start_pos.y = YPOS(0);
		end_pos.x = XPOS(x);
		end_pos.y = YPOS(Y_COUNT - 1);
		// (x,0) 점과 (x, Y_COUNT-1) 점을 연결하는 선을 그린다
		gp_render_target->DrawLine(start_pos, end_pos, p_line_brush, (float)1.0f);
	}
	// 수평으로 19개의 선을 그린다
	for (int y = 0; y < Y_COUNT; y++) {
		start_pos.x = XPOS(0);
		start_pos.y = YPOS(y);
		end_pos.x = XPOS(X_COUNT - 1);
		end_pos.y = YPOS(y);
		// (0,y) 점과 (X_COUNT -1, y) 점을 연결하는 선을 그린다
		gp_render_target->DrawLine(start_pos, end_pos, p_line_brush, (float)1.0f);
	}

	// 타원 그리기
	D2D1_ELLIPSE dol_region;
	dol_region.radiusX = (float)HALF_INTERVAL;				// 타원 반지름(x)
	dol_region.radiusY = (float)HALF_INTERVAL;				// 타원 반지름(y)

	// 바둑판 각 위치의 g_dol 플래그를 통해 어떤 바둑돌이 놓여있는지 체크한다 (0:없음, 1:검은돌, 2:흰돌)
	for (int y = 0; y < Y_COUNT; y++) {
		for (int x = 0; x < X_COUNT; x++) {
			if (g_dol[y][x] != 0) {					// 돌이 있다면
				dol_region.point.x = XPOS(x);
				dol_region.point.y = YPOS(y);
				if (g_dol[y][x] == 1) {
					gp_render_target->FillEllipse(dol_region, p_black_brush);				// 검은돌
				}
				else if(g_dol[y][x] == 2) {
					gp_render_target->FillEllipse(dol_region, p_white_brush);				// 흰돌
				}
			}
		}
	}

	// 리소스 해제
	p_line_brush->Release();
	p_line_brush = NULL;
	p_black_brush->Release();
	p_black_brush = NULL;
	p_white_brush->Release();
	p_white_brush = NULL;
	// D2D 그리기 중지
	gp_render_target->EndDraw();
}

void OnLButtonDown(HWND ah_wnd, BOOL a_double_click, int a_x, int a_y, UINT a_key_flags)
{
	// 클릭 위치를 기준으로 가장 가까운 위치에 생성
	if (a_x > (XPOS(0) - HALF_INTERVAL) && a_y > (YPOS(0) - HALF_INTERVAL) && a_x < (XPOS(X_COUNT - 1) + HALF_INTERVAL) && a_y < (YPOS(Y_COUNT - 1) + HALF_INTERVAL)) {
		// 여백을 빼고, 줄 간격으로 나누면 0~18 사이로 얻는다
		// 클릭 위치와 가장 가까운 줄(가운데)로 이동할 수 있도록 줄간격의 절반만큼 더한 후에 줄간격으로 나눈다
		int x = (a_x - START_X + HALF_INTERVAL) / INTERVAL;
		int y = (a_y - START_Y + HALF_INTERVAL) / INTERVAL;
		if (g_dol[y][x] == 0) {
			// 돌없음 0, 검은돌 1, 흰돌 2
			g_dol[y][x] = g_step + 1;
			g_step = !g_step;

			InvalidateRect(ah_wnd, NULL, TRUE);				// 클릭시 화면 갱신
		}
	}
}

BOOL OnCreate(HWND ah_wnd, LPCREATESTRUCT lpCreateStruct)
{
	RECT r;
	GetClientRect(ah_wnd, &r);

	HRESULT hr = gp_factory->CreateHwndRenderTarget(RenderTargetProperties(), HwndRenderTargetProperties(ah_wnd, SizeU(r.right, r.bottom)), &gp_render_target);
	if (SUCCEEDED(hr)) {
		return TRUE;
	}
	return FALSE;
}

void OnDestroy(HWND ah_wnd)
{
	PostQuitMessage(0);
}

