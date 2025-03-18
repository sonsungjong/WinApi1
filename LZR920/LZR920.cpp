#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#pragma comment(lib, "winmm.lib") // 타이머 등 필요 시

// ── 설정 ───────────────────────────────────────────────
#define NUM_POINTS 274
#define HEADER_SIZE 4
#define CMD_MDI 50011   // MDI 측정 데이터 프레임 명령 코드
// 헤더: 센서 프로토콜에 따르면 0xFFFEFDFC, LSB first → 바이트 순서: 0xFC, 0xFD, 0xFE, 0xFF
BYTE g_Header[HEADER_SIZE] = { 0xFC, 0xFD, 0xFE, 0xFF };

HANDLE hComm = INVALID_HANDLE_VALUE;
CRITICAL_SECTION g_cs;
double g_x[NUM_POINTS] = { 0 }, g_y[NUM_POINTS] = { 0 };
volatile BOOL g_dataAvailable = FALSE;

BOOL sendSingleShot(HANDLE hComm) {
    BYTE frame[10] = { 0xFC,0xFD,0xFE,0xFF,  // Header
                      0x02,0x00,           // SIZE = 2
                      0x17,0xC3 };          // CMD = 50011
    WORD chk = frame[6] + frame[7];
    frame[8] = chk & 0xFF;
    frame[9] = (chk >> 8) & 0xFF;

    DWORD written;
    return WriteFile(hComm, frame, sizeof(frame), &written, NULL) && (written == sizeof(frame));
}

// ── 시리얼 포트 열기 ─────────────────────────────────
BOOL OpenSerialPort(const char* portName) {
    hComm = CreateFileA(portName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (hComm == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(hComm, &dcb)) {
        return FALSE;
    }
    dcb.BaudRate = 460800;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    if (!SetCommState(hComm, &dcb)) {
        return FALSE;
    }
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hComm, &timeouts);
    return TRUE;
}

// ── 시리얼 프레임 읽기 ────────────────────────────────
// 프레임 구조: HEADER(4) | SIZE(2) | CMD+DATA (SIZE bytes) | CHK(2)
// DATA는 측정 데이터(각 포인트 2바이트, 총 274×2=548바이트)를 포함한다고 가정.
BOOL ReadSerialFrame(BYTE** ppFrame, DWORD* pMsgSize) {
    BYTE byte;
    BYTE headerBuf[HEADER_SIZE] = { 0 };
    int idx = 0;
    // 헤더 찾기: 한 바이트씩 읽으면서 최근 4바이트가 HEADER와 일치하는지 확인
    while (TRUE) {
        DWORD bytesRead;
        if (!ReadFile(hComm, &byte, 1, &bytesRead, NULL) || bytesRead == 0)
            return FALSE;
        // shift headerBuf
        for (int i = 0; i < HEADER_SIZE - 1; i++) {
            headerBuf[i] = headerBuf[i + 1];
        }
        headerBuf[HEADER_SIZE - 1] = byte;
        if (memcmp(headerBuf, g_Header, HEADER_SIZE) == 0)
            break;
    }
    // SIZE (2바이트)
    BYTE sizeBytes[2];
    DWORD bytesRead;
    if (!ReadFile(hComm, sizeBytes, 2, &bytesRead, NULL) || bytesRead != 2)
        return FALSE;
    WORD msgSize = *(WORD*)sizeBytes;  // 리틀 엔디안
    // 메시지 읽기 (CMD+DATA)
    BYTE* msg = (BYTE*)malloc(msgSize);
    if (!msg)
        return FALSE;
    if (!ReadFile(hComm, msg, msgSize, &bytesRead, NULL) || bytesRead != msgSize) {
        free(msg);
        return FALSE;
    }
    // CHK (2바이트)
    BYTE chkBytes[2];
    if (!ReadFile(hComm, chkBytes, 2, &bytesRead, NULL) || bytesRead != 2) {
        free(msg);
        return FALSE;
    }
    WORD recvChk = *(WORD*)chkBytes;
    WORD calcChk = 0;
    for (DWORD i = 0; i < msgSize; i++) {
        calcChk += msg[i];
    }
    if ((calcChk & 0xFFFF) != recvChk) {
        free(msg);
        return FALSE;
    }
    *ppFrame = msg;
    *pMsgSize = msgSize;
    return TRUE;
}

// ── 센서 데이터 파싱 ───────────────────────────────────
// 메시지의 처음 2바이트는 CMD, 나머지는 DATA (각 측정값은 2바이트, 총 NUM_POINTS*2 바이트)
BOOL ParseSensorData(BYTE* msg, DWORD msgSize) {
    if (msgSize < 2 + NUM_POINTS * 2)
        return FALSE;
    WORD cmd = *(WORD*)msg;
    if (cmd != CMD_MDI)
        return FALSE;
    WORD* data = (WORD*)(msg + 2);
    // 각 포인트 데이터 변환: 가정 - 측정값은 mm 단위
    for (int i = 0; i < NUM_POINTS; i++) {
        double distance = (double)data[i];
        // 각도 계산: -48°에서 +48°까지 균등 분포
        double angle_deg = -48.0 + (96.0 / (NUM_POINTS - 1)) * i;
        double angle_rad = angle_deg * 3.14159265 / 180.0;
        EnterCriticalSection(&g_cs);
        g_x[i] = distance * cos(angle_rad);
        g_y[i] = distance * sin(angle_rad);
        LeaveCriticalSection(&g_cs);
    }
    g_dataAvailable = TRUE;
    return TRUE;
}

// ── 시리얼 읽기 스레드 ──────────────────────────────────
DWORD WINAPI SerialThread(LPVOID lpParam) {
    while (1) {
        BYTE* frame = NULL;
        DWORD msgSize = 0;
        if (ReadSerialFrame(&frame, &msgSize)) {
            ParseSensorData(frame, msgSize);
            free(frame);
        }
    }
    return 0;
}

// ── GDI 그리기용 윈도우 프로시저 ─────────────────────────
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;
    static HFONT hFont;
    switch (msg) {
    case WM_CREATE:
        InitializeCriticalSection(&g_cs);
        SetTimer(hwnd, 1, 50, NULL); // 50ms 타이머
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_RETURN) {  // 엔터키를 누르면 센서에 단일 측정 명령 전송
            if (sendSingleShot(hComm))
                MessageBox(hwnd, L"단일 측정 명령 전송 성공", L"정보", MB_OK);
            else
                MessageBox(hwnd, L"단일 측정 명령 전송 실패", L"오류", MB_OK);
        }
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        // 배경 지우기
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        // 센서 데이터가 있으면 그리기 (좌표 변환: 화면 중심 (300,300), 스케일: 0.1 pixel per mm)
        EnterCriticalSection(&g_cs);
        if (g_dataAvailable) {
            for (int i = 0; i < NUM_POINTS; i++) {
                int x = 300 + (int)(g_x[i] * 0.1);
                int y = 300 - (int)(g_y[i] * 0.1); // Y축 반전
                Ellipse(hdc, x - 2, y - 2, x + 2, y + 2);
            }
        }
        LeaveCriticalSection(&g_cs);
        EndPaint(hwnd, &ps);
        return 0;
    case WM_TIMER:
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    case WM_DESTROY:
        DeleteCriticalSection(&g_cs);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ── WinMain ─────────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t* className = L"SensorWindowClass";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(className, L"실시간 센서 데이터", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
        NULL, NULL, hInstance, NULL);
    if (!hwnd) return 0;
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    if (!OpenSerialPort("\\\\.\\COM8")) {
        MessageBox(hwnd, L"시리얼 포트를 열 수 없습니다.", L"Error", MB_ICONERROR);
        return 1;
    }

    // 시리얼 읽기 스레드 시작
    HANDLE hThread = CreateThread(NULL, 0, SerialThread, NULL, 0, NULL);
    if (!hThread) {
        MessageBox(hwnd, L"스레드 생성 실패.", L"Error", MB_ICONERROR);
        return 1;
    }

    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CloseHandle(hThread);
    CloseHandle(hComm);
    return (int)msg.wParam;
}
