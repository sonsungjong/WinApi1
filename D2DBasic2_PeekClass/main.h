#pragma once

#include "resource.h"

#define X_COUNT			19
#define Y_COUNT			19

// ���� ����
#define START_X			50
#define START_Y			50

// �� ����
#define INTERVAL			26

// �ٵϵ� ������ (������ ����)
#define HALF_INTERVAL			INTERVAL/2

// �ٵϵ� ��ġ ���� (Ŭ����ġ ����)
#define XPOS(x)				(float)(START_X + (x)*INTERVAL)
#define YPOS(y)					(float)(START_Y + (y)*INTERVAL)

void D2DCustomDraw(ID2D1BitmapRenderTarget* backBuffer);
