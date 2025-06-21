#include "framework.h"
#include <windows.h>
#include <imm.h>
#include <string>
#pragma comment(lib, "imm32.lib")


std::wstring g_text;            // 실제 입력된 텍스트
std::wstring g_composition;     // IME 조합 중 텍스트
int g_cursor = 0;               // 커서 위치
bool g_focused = false;         // 포커스 상태
HWND g_hwnd = nullptr;
bool g_skipNextChar = false; // 조합확정 중복입력 방지

void HandleIMEComposition(HWND hwnd, LPARAM lParam) {
    HIMC hIMC = ImmGetContext(hwnd);
    if (lParam & GCS_COMPSTR) {
        LONG size = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, nullptr, 0);
        if (size > 0) {
            wchar_t* buffer = new wchar_t[(size / 2) + 1]();
            ImmGetCompositionStringW(hIMC, GCS_COMPSTR, buffer, size);
            buffer[size / 2] = 0;
            g_composition = buffer;
            delete[] buffer;
        }
        else {
            g_composition.clear();
        }
    }
    if (lParam & GCS_RESULTSTR) {
        LONG size = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, nullptr, 0);
        if (size > 0) {
            wchar_t* buffer = new wchar_t[(size / 2) + 1]();
            ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, buffer, size);
            buffer[size / 2] = 0;
            g_text.insert(g_cursor, buffer);
            g_cursor += wcslen(buffer);
            g_composition.clear();
            delete[] buffer;
            g_skipNextChar = true; // 조합확정 직후 WM_CHAR 1회 무시
        }
    }
    ImmReleaseContext(hwnd, hIMC);
    InvalidateRect(hwnd, nullptr, TRUE);
}

void RenderEditControl(HWND hwnd, HDC hdc) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    FillRect(hdc, &rc, (HBRUSH)(g_focused ? COLOR_WINDOW + 1 : COLOR_BTNFACE + 1));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));

    // 전체 문자열: 실제입력 + 조합문자
    std::wstring display = g_text;
    int drawCursor = g_cursor;
    if (!g_composition.empty()) {
        display.insert(g_cursor, g_composition);
    }
    TextOutW(hdc, 4, 4, display.c_str(), display.length());

    // 커서 표시 (조합중이면 조합뒤에, 아니면 텍스트 뒤에)
    SIZE sz = { 0 };
    GetTextExtentPoint32W(hdc, display.c_str(), drawCursor + (int)g_composition.length(), &sz);
    if (g_focused) {
        // 간단한 커서(직선) 구현
        MoveToEx(hdc, 4 + sz.cx, 4, nullptr);
        LineTo(hdc, 4 + sz.cx, 24);
    }
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) 
    {
        case WM_SETFOCUS:
        {
            g_focused = true;
            ImmAssociateContext(hwnd, ImmGetContext(hwnd)); // IME 활성화
            InvalidateRect(hwnd, nullptr, TRUE);
            break;
        }
        case WM_KILLFOCUS:
        {
            g_focused = false;
            g_composition.clear();
            InvalidateRect(hwnd, nullptr, TRUE);
            break;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RenderEditControl(hwnd, hdc);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_CHAR:
        {
            if (g_skipNextChar) {
                g_skipNextChar = false; // 조합확정 직후 1회만 스킵
                break;
            }

            if (wParam == VK_ESCAPE) {

            }
            else if (wParam == VK_BACK) {
                if (!g_composition.empty()) break; // 조합 중에는 Backspace 무시
                if (g_cursor > 0 && !g_text.empty()) {
                    g_text.erase(g_cursor - 1, 1);
                    --g_cursor;
                }
            }
            else if (wParam == VK_RETURN) {
                // 엔터 키 처리 (원하는대로)
            }
            else {
                if (!g_composition.empty()) break; // 조합중 일반키 무시
                g_text.insert(g_cursor, 1, (wchar_t)wParam);
                ++g_cursor;
            }
            InvalidateRect(hwnd, nullptr, TRUE);
            break;
        }
        case WM_KEYDOWN:
        {
            if (wParam == VK_LEFT && g_cursor > 0) {
                --g_cursor;
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            else if (wParam == VK_RIGHT && g_cursor < (int)g_text.length()) {
                ++g_cursor;
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            break;
        }
        case WM_IME_STARTCOMPOSITION:
        {
            g_composition.clear();

            InvalidateRect(hwnd, nullptr, TRUE);
            break;
        }
        case WM_IME_COMPOSITION:
        {
            HandleIMEComposition(hwnd, lParam);
            break;
        }
        case WM_IME_ENDCOMPOSITION:
        {
            g_composition.clear();
            InvalidateRect(hwnd, nullptr, TRUE);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
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

	return (int)msg.wParam;
}

