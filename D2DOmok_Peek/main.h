#pragma once

#include "resource.h"

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

void OnPaint(HWND ah_wnd);
void OnLButtonDown(HWND ah_wnd, BOOL a_double_click, int x, int y, UINT a_key_flags);
void OnDestroy(HWND ah_wnd);

void D2DCustomDraw();
void D2DRender();