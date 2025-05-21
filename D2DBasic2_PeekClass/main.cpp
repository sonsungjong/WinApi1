
#include "pch.h"
#include "framework.h"
#include "main.h"
#include "D2DRenderManager.h"

HINSTANCE g_hInst;
HWND g_hWnd;
ID2D1BitmapRenderTarget* gp_back_buffer;


// 바둑돌
unsigned char g_dol[Y_COUNT][X_COUNT];
unsigned char g_step;           // 0:검은돌차례, 1:흰돌차례

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (WM_PAINT == msg)
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0; // 직접 렌더링하므로 아무것도 안 함
    }
    else if (WM_LBUTTONDOWN == msg)
    {
        int click_x = LOWORD(lParam);
        int click_y = HIWORD(lParam);

        // 클릭 위치를 기준으로 가장 가까운 위치에 생성
        if (click_x > (XPOS(0) - HALF_INTERVAL) && click_y > (YPOS(0) - HALF_INTERVAL) && click_x < (XPOS(X_COUNT - 1) + HALF_INTERVAL) && click_y < (YPOS(Y_COUNT - 1) + HALF_INTERVAL)) {
            // 여백을 빼고, 줄 간격으로 나누면 0~18 사이로 얻는다
            // 클릭 위치와 가장 가까운 줄(가운데)로 이동할 수 있도록 줄간격의 절반만큼 더한 후에 줄간격으로 나눈다
            int x = (click_x - START_X + HALF_INTERVAL) / INTERVAL;
            int y = (click_y - START_Y + HALF_INTERVAL) / INTERVAL;
            if (g_dol[y][x] == 0) {
                // 돌없음 0, 검은돌 1, 흰돌 2
                g_dol[y][x] = g_step + 1;
                g_step = !g_step;

                //InvalidateRect(ah_wnd, NULL, TRUE);				// 클릭시 화면 갱신 (PeekMessage로 변경해서 제거)
            }
        }
    }
    else if (WM_DESTROY == msg)
    {
        PostQuitMessage(0);
    }
    else if (WM_SIZE == msg)
    {
        
    }
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	g_hInst = hInstance;

    SetProcessDPIAware();                   // 1. DPI 반응성 추가
    (void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);       // 2. COM라이브러리 초기화
    

    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = g_hInst;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(243, 243, 243));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"D2D_Omok";
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    // 창 크기
    RECT rc = { 0, 0, 1920, 1080 };
    ::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);            // 여백보정

    g_hWnd = ::CreateWindowW(L"D2D_Omok", L"타이틀", WS_OVERLAPPEDWINDOW,
        rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

    ShowWindow(g_hWnd, TRUE);
    UpdateWindow(g_hWnd);

    D2DRenderManager::getInstance().init();             // 3. 초기화, 팩토리 생성

    RECT r;
    GetClientRect(g_hWnd, &r);
    D2DRenderManager::getInstance().addTargetWnd(g_hWnd, r);      // 4. 윈도우 및 범위지정
    gp_back_buffer = D2DRenderManager::getInstance().getBackBuffer(g_hWnd);

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            D2DCustomDraw(gp_back_buffer);

			D2DRenderManager::getInstance().updateViewAll();
        }
    }

    //D2DRenderManager::getInstance().removeTargetWnd(g_hWnd);
    D2DRenderManager::getInstance().destroy();
    CoUninitialize();

    return msg.wParam;
}

void D2DCustomDraw(ID2D1BitmapRenderTarget* backBuffer)
{
    if (backBuffer)
    {
        backBuffer->BeginDraw();
        backBuffer->Clear(D2D1::ColorF(0.95686f, 0.69f, 0.3019f));				// 배경색

        // D2D Brush 생성
        ID2D1SolidColorBrush* p_line_brush, * p_black_brush, * p_white_brush;
        backBuffer->CreateSolidColorBrush(D2D1::ColorF(0.095686f, 0.069f, 0.03019f), &p_line_brush);
        backBuffer->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), &p_black_brush);
        backBuffer->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), &p_white_brush);

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
                backBuffer->DrawLine(start_pos, end_pos, p_line_brush, (float)1.0f);
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
                backBuffer->DrawLine(start_pos, end_pos, p_line_brush, (float)1.0f);
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
                            backBuffer->FillEllipse(dol_region, p_black_brush);				// 검은돌
                        }
                    }
                    else if (g_dol[y][x] == 2) {
                        if (p_white_brush) {
                            backBuffer->FillEllipse(dol_region, p_white_brush);				// 흰돌
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
        backBuffer->EndDraw();
    }
}

void OnSize(HWND hWnd, UINT state, int cx, int cy)
{

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