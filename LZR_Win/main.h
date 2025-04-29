#pragma once

#include "resource.h"

typedef struct STDoubleBuffer
{
	HDC m_hMemDC;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	int m_clientWidth;
	int m_clientHeight;
} ST_DoubleBuffer;

typedef struct STViewRgn
{
	RECT rect;
	int width;
	int height;
} ST_ViewRgn;

void ViewRgn_SetRgn(ST_ViewRgn* stRgn, int _sx, int _sy, int _ex, int _ey);
void initPos();
void createControls(HWND hWnd);
float calculateWidth(float fPercentage);
float calculateHeight(float fPercentage);

void DoubleBuffer_init(ST_DoubleBuffer* pBuffer, HWND hWnd);
void DoubleBuffer_Resize(ST_DoubleBuffer* pBuffer, HWND hWnd, int newWidth, int newHeight);
void DoubleBuffer_Paint(ST_DoubleBuffer* pBuffer, HWND hWnd, PAINTSTRUCT* ps);
void DoubleBuffer_Destroy(ST_DoubleBuffer* pBuffer);