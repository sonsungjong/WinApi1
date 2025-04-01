#pragma once

#include "resource.h"

#define CMD_MDI 0x5011


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL PreTranslateMessage(MSG* pMsg);
bool openSerialPort(const wchar_t* portName = L"\\\\.\\COM8", DWORD baudRate = 460800);
void recvSerialPort();
void DrawGDI(HDC hdc);




void OnPaint(HWND hWnd);
int OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);			// [0] 정상 [-1] 오류
void OnDestroy(HWND hWnd);
void OnClose(HWND hWnd);
void OnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT codeNotify);
void OnTimer(HWND hWnd, UINT_PTR id);
