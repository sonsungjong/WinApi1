#pragma once

#include "resource.h"

#define IDC_EDIT_PORT_NUMBER										30001
#define IDC_BUTTON_CONN_PORT										30002
#define IDC_BUTTON_MODE_MEASUREMENT						30003
#define IDC_BUTTON_MODE_CONFIGURATION						30004
#define IDC_EDIT_START_SPOT											30005
#define IDC_EDIT_END_SPOT												30006
#define IDC_COMBO_DISTANCE											30007
#define IDC_BUTTON_SETTING_CHANGE								30008
#define IDC_BUTTON_SETTING_RESET								30009
#define IDC_BUTTON_SETTING_SAVE									30010

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