#include "pch.h"
#include "main.h"

// DX11
#include <wrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "SimpleMath.h"
#include <dxgi.h>
// D2D
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
// GDI+
#include <gdiplus.h>

// DX11
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
// D2D
#pragma comment(lib, "d2d1.lib")
// GDI+
#pragma comment(lib, "gdiplus.lib")

#ifdef _DEBUG

#else

#endif

HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;

// D2D 객체
ID2D1Factory* gp_factory = nullptr;										// 전역적으로 한번만 생성
ID2D1HwndRenderTarget* gp_render_target = nullptr;				// 각 창마다 랜더타겟 생성
ID2D1BitmapRenderTarget* gp_back_buffer = nullptr;				// 각 창마다 더블버퍼 생성
ID2D1Bitmap* gp_copy_bitmap = nullptr;								// 각 창마다 더블버퍼 옮기기용 비트맵 생성

// 바둑돌이 있는지 여부
unsigned char g_dol[Y_COUNT][X_COUNT];

// 검은돌, 흰돌 순서 (0:검은돌 차례, 1:흰돌 차례)
unsigned char g_step;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hWnd, WM_SIZE, OnSize);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    g_hInst = hInstance;                    // 인스턴스 핸들을 전역 변수에 저장합니다.

    // D2D 라이브러리 초기화
	SetProcessDPIAware();
    (void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    // Register Class
	const TCHAR wnd_class_name[] = _T("D2DPeek");
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = g_hInst;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = wnd_class_name;
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    // 창의 크기를 지정
    RECT rc = { 50, 50, 650, 650 };
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
    ::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);              // 여백보정

    // 윈도우 생성 후 핸들값 반환
	g_hWnd = ::CreateWindowW(wnd_class_name, L"My Window1", WS_OVERLAPPEDWINDOW,
		rc.left, rc.top, width, height, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(g_hWnd, true);
    UpdateWindow(g_hWnd);

    // DirectX 엔진 초기화
	//CEngine::getInstance()->init(g_hWnd, SimpleMath::Vector2(1280, 768));

	// D2D factory 생성
	if (S_OK != D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &gp_factory)) {
		return 0;
	}
	// D2D 랜더타겟 지정 및 범위설정 r
	RECT r;
	GetClientRect(g_hWnd, &r);

	if (gp_factory) {
		HRESULT hr = gp_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(g_hWnd, D2D1::SizeU(r.right, r.bottom)), &gp_render_target);
		if (SUCCEEDED(hr)) {
			hr = gp_render_target->CreateCompatibleRenderTarget(&gp_back_buffer);
			if (SUCCEEDED(hr)) {
			}
		}
	}

    // 단축키 테이블
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D2DOMOKPEEK));

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        // DirectX는 항상 렌더링 될 수 있게 PeekMessage를 사용해야한다
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 메시지 처리
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // 엔진 렌더링 수행
			D2DCustomDraw();

			D2DRender();
        }
    }

    // D2D 해제
	if (gp_copy_bitmap) { 
		gp_copy_bitmap->Release(); 
		gp_copy_bitmap = NULL;
	}
	if (gp_back_buffer) {
		gp_back_buffer->Release();
		gp_back_buffer = NULL;
	}
	if (gp_render_target) {
		gp_render_target->Release();
		gp_render_target = NULL;
	}
	if (gp_factory) { 
		gp_factory->Release();
		gp_factory = NULL;
	}
    CoUninitialize();

    return (int)msg.wParam;
}

void D2DCustomDraw()
{
	if (gp_render_target && gp_back_buffer)
	{
		gp_back_buffer->BeginDraw();
		gp_back_buffer->Clear(D2D1::ColorF(0.95686f, 0.69f, 0.3019f));				// 배경색

		// D2D Brush 생성
		ID2D1SolidColorBrush* p_line_brush, * p_black_brush, * p_white_brush;
		gp_back_buffer->CreateSolidColorBrush(D2D1::ColorF(0.095686f, 0.069f, 0.03019f), &p_line_brush);
		gp_back_buffer->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), &p_black_brush);
		gp_back_buffer->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), &p_white_brush);

		D2D1_POINT_2F start_pos, end_pos;

		// 선 그리기
		// 수직으로 19개의 선을 그린다
		for (int x = 0; x < X_COUNT; x++) {
			start_pos.x = XPOS(x);
			start_pos.y = YPOS(0);
			end_pos.x = XPOS(x);
			end_pos.y = YPOS(Y_COUNT - 1);
			// (x,0) 점과 (x, Y_COUNT-1) 점을 연결하는 선을 그린다
			if (p_line_brush)
			{
				gp_back_buffer->DrawLine(start_pos, end_pos, p_line_brush, (float)1.0f);
			}
		}
		// 수평으로 19개의 선을 그린다
		for (int y = 0; y < Y_COUNT; y++) {
			start_pos.x = XPOS(0);
			start_pos.y = YPOS(y);
			end_pos.x = XPOS(X_COUNT - 1);
			end_pos.y = YPOS(y);
			// (0,y) 점과 (X_COUNT -1, y) 점을 연결하는 선을 그린다
			if (p_line_brush) {
				gp_back_buffer->DrawLine(start_pos, end_pos, p_line_brush, (float)1.0f);
			}
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
						if (p_black_brush) {
							gp_back_buffer->FillEllipse(dol_region, p_black_brush);				// 검은돌
						}
					}
					else if (g_dol[y][x] == 2) {
						if (p_white_brush) {
							gp_back_buffer->FillEllipse(dol_region, p_white_brush);				// 흰돌
						}
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
		// 백버퍼 그리기 완료
		gp_back_buffer->EndDraw();

		// 백 버퍼를 메인 렌더 타겟에 복사
		gp_back_buffer->GetBitmap(&gp_copy_bitmap);
	}
}

void D2DRender()
{
	if (gp_factory && gp_copy_bitmap)
	{
		gp_render_target->BeginDraw();
		gp_render_target->Clear();
		gp_render_target->DrawBitmap(gp_copy_bitmap);
		gp_render_target->EndDraw();

		gp_copy_bitmap->Release();
		gp_copy_bitmap = nullptr;
	}
}

void OnPaint(HWND ah_wnd)
{
	PAINTSTRUCT ps;
	if (g_hWnd) {
		HDC hdc = BeginPaint(g_hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...


		EndPaint(g_hWnd, &ps);
	}
}

void OnSize(HWND hWnd, UINT state, int cx, int cy)
{
	if (gp_render_target) {
		gp_render_target->Resize(D2D1::SizeU(cx, cy));
	}
	if (gp_back_buffer) {
		gp_back_buffer->Release();
		gp_back_buffer = nullptr;
	}
	if (gp_render_target) {
		gp_render_target->CreateCompatibleRenderTarget(&gp_back_buffer);
	}
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

			//InvalidateRect(ah_wnd, NULL, TRUE);				// 클릭시 화면 갱신 (PeekMessage로 변경해서 제거)
		}
	}
}


void OnDestroy(HWND ah_wnd)
{
	PostQuitMessage(0);
}