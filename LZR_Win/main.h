#pragma once

#include "resource.h"

#define IDC_EDIT_PORT_NUMBER										30001
#define IDC_BUTTON_CONN_PORT										30002
#define IDC_BUTTON_MODE_MEASUREMENT						30003
#define IDC_BUTTON_MODE_CONFIGURATION						30004
#define IDC_EDIT_START_SPOT											30005
#define IDC_EDIT_END_SPOT												30006
#define IDC_COMBO_APD_DISTANCE									30007
#define IDC_BUTTON_SETTING_CHANGE								30008
#define IDC_BUTTON_SETTING_RESET								30009
#define IDC_BUTTON_SETTING_SAVE									30010
#define IDC_EDIT_MAX_DISTANCE										30011
#define IDC_BUTTON_RED_LASER										30012
#define ID_PAINT_MDI														40001
#define ID_TIMER_DETECTION_OFF									40002
#define IDC_EDIT_DETECTION_PERCENTAGE									40002
#define IDC_EDIT_DETECTION_SCALE									40002
#define IDC_EDIT_DETECTION_INTERVAL									40002

typedef struct STDoubleBuffer
{
	HDC m_hMemDC;
	HBITMAP m_hBitmap;
	HBITMAP m_hOldBitmap;
	int m_clientWidth;
	int m_clientHeight;
} ST_DoubleBuffer;

typedef struct STMDI_DATA{
	unsigned short mdi[4][274];  // Plane 0~3, 거리값
} ST_MDI_DATA;

typedef struct STViewRgn
{
	RECT rect;
	int width;
	int height;
} ST_ViewRgn;

// 각 Plane 색상
COLORREF PLANE_COLORS[4] = {
	RGB(255, 255, 255),     // Plane 0 - 하양
	RGB(255, 212, 0),     // Plane 1 - 노랑
	RGB(0, 255, 0),     // Plane 2 - 초록
	RGB(238, 130, 238)    // Plane 3 - 바이올렛
};

void recvFunction();

void ViewRgn_SetRgn(ST_ViewRgn* stRgn, int _sx, int _sy, int _ex, int _ey);
void initPos();
void createControls(HWND hWnd);
float calculateWidth(float fPercentage);
float calculateHeight(float fPercentage);

void DoubleBuffer_init(ST_DoubleBuffer* pBuffer, HWND hWnd);
void DoubleBuffer_Resize(ST_DoubleBuffer* pBuffer, HWND hWnd, int newWidth, int newHeight);
void DoubleBuffer_Paint(ST_DoubleBuffer* pBuffer, HWND hWnd, PAINTSTRUCT* ps);
void DoubleBuffer_Destroy(ST_DoubleBuffer* pBuffer);
void checkMDIDifferenceAndTriggerDetection(unsigned short prev[4][274], unsigned short curr[4][274]);