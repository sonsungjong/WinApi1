#pragma once

#include "resource.h"

void OnPaint(HWND);
//void OnLButtonDown(HWND, BOOL, int, int, UINT);
BOOL OnCreate(HWND, LPCREATESTRUCT);
void OnDestroy(HWND);
void OnCommand(HWND ah_wnd, int id, HWND ah_wnd_ctrl, UINT codeNotify);