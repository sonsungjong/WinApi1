#ifndef D2D_OMOK_H_
#define D2D_OMOK_H_

#include "resource.h"

void OnPaint(HWND ah_wnd);
void OnLButtonDown(HWND ah_wnd, BOOL a_double_click, int x, int y, UINT a_key_flags);
BOOL OnCreate(HWND ah_wnd, LPCREATESTRUCT lpCreateStruct);
void OnDestroy(HWND ah_wnd);

#endif