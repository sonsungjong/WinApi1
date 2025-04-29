#include "pch.h"
#include "framework.h"
#include "main.h"

HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
HWND g_hWnd;
float g_dpi_response;
int g_wndWidth;
int g_wndHeight;
int g_screenWidth;
int g_screenHeight;
int g_posX;
int g_posY;
BOOL g_isCreated = false;
ST_DoubleBuffer g_doubleBuffer;

char g_comName[16] = { 0, };
ST_ViewRgn g_rgnTopBar;
ST_ViewRgn g_rgnTopBarTitle;

ST_ViewRgn g_rgnMDIViewer;
ST_ViewRgn g_rgnSettingViewerBox;
ST_ViewRgn g_rgnConnModeBox;
ST_ViewRgn g_rgnSettingChangeBox;

ST_ViewRgn g_rgnTextConnTitle;				// 센서 연결
ST_ViewRgn g_rgnTextPortNumber;				// 포트번호 COM
ST_ViewRgn g_rgnEditPortNumber;
ST_ViewRgn g_rgnBtnConnPort;							// 연결
ST_ViewRgn g_rgnTextModeTitle;							// 센서 모드 변경
ST_ViewRgn g_rgnBtnModeMeasurement;
ST_ViewRgn g_rgnBtnModeConfiguration;

ST_ViewRgn g_rgnTextSetStartSpot;					// 측정반경 시작지점 [0~273]
ST_ViewRgn g_rgnEditSetStartSpot;					// 시작지점 0~273
ST_ViewRgn g_rgnTextSetEndSpot;					// 측정반경 끝지점 [1~274]
ST_ViewRgn g_rgnEditSetEndSpot;					// 끝지점 1~274
ST_ViewRgn g_rgnTextSetDistance;					// 측정 거리
ST_ViewRgn g_rgnComboSetDistance;				// 8m, 12m, 16m 거리
ST_ViewRgn g_rgnBtnSetChangeSetting;				// 변경 (RAM)
ST_ViewRgn g_rgnBtnSetInitSetting;					// 초기화 (RAM)
ST_ViewRgn g_rgnBtnSetSaveSetting;				// 변경사항 저장 (EPPROM)

ST_ViewRgn g_rgnTextCurMode;				// 측정모드, 설정모드
//ST_ViewRgn g_rgnTextCurModeValue;				// 현재모드
ST_ViewRgn g_rgnTextBaudRate;				// 현재 전송 속도
ST_ViewRgn g_rgnTextBaudRateValue;				// 현재 전송 속도
ST_ViewRgn g_rgnTextPlane0;				// Plane0 활성여부
ST_ViewRgn g_rgnTextPlane0Value;				// Plane0 활성여부
ST_ViewRgn g_rgnTextPlane1;				// Plane1 활성여부
ST_ViewRgn g_rgnTextPlane1Value;				// Plane1 활성여부
ST_ViewRgn g_rgnTextPlane2;				// Plane2 활성여부
ST_ViewRgn g_rgnTextPlane2Value;				// Plane2 활성여부
ST_ViewRgn g_rgnTextPlane3;				// Plane3 활성여부
ST_ViewRgn g_rgnTextPlane3Value;				// Plane3 활성여부
ST_ViewRgn g_rgnTextStartingSpot;				// 측정 시작지점
ST_ViewRgn g_rgnTextStartingSpotValue;				// 측정 시작지점
ST_ViewRgn g_rgnTextStartingAngle;				// 측정 시작각도
ST_ViewRgn g_rgnTextStartingAngleValue;				// 측정 시작각도
ST_ViewRgn g_rgnTextEndSpot;				// 측정 종료지점
ST_ViewRgn g_rgnTextEndSpotValue;				// 측정 종료지점
ST_ViewRgn g_rgnTextEndAngle;				// 측정 종료각도
ST_ViewRgn g_rgnTextEndAngleValue;				// 측정 종료각도
ST_ViewRgn g_rgnAPD_Distance;				// 측정 최대사거리
ST_ViewRgn g_rgnAPD_DistanceValue;				// 측정 최대사거리

RECT g_wndRect;
const wchar_t g_szTitle[64] = L"LZR920";
const wchar_t g_szPort[64] = L"포트";
const wchar_t g_szConnModeBoxTitle[64] = L"센서 연결";
const wchar_t g_szSetStartSpot[64] = L"시작 반경 [0~273]";
const wchar_t g_szSetEndSpot[64] = L"종료 반경 [1~274]";
const wchar_t g_szSetDistance[64] = L"최대 측정 거리";

const wchar_t g_szBaudRate[64] = L"전송속도";
const wchar_t g_szStartingSpot[64] = L"시작반경";
const wchar_t g_szStartingSpotAngle[64] = L"시작각도";
const wchar_t g_szEndSpot[64] = L"종료반경";
const wchar_t g_szEndSpotAngle[64] = L"종료각도";
const wchar_t g_szAPDDistance[64] = L"최대거리";

wchar_t g_szCurMode[64] = L"측정모드";
wchar_t g_szBaudRateValue[64] = L"460800";
wchar_t g_szStartingSpotValue[64] = L"0";
wchar_t g_szStartingSpotAngleValue[64] = L"-48º";
wchar_t g_szEndSpotAngleValue[64] = L"48º";
wchar_t g_szEndSpotValue[64] = L"274";
wchar_t g_szAPDDistanceValue[64] = L"12m";

HWND g_hEditPortNumber;
HWND g_hBtnConnPort;
HWND g_hBtnModeMeasurement;
HWND g_hBtnModeConfiguration;
HWND g_hEditStartSpot;
HWND g_hEditEndSpot;
HWND g_hComboDistance;
HWND g_hBtnChangeSetting;
HWND g_hBtnInitSetting;
HWND g_hBtnSaveSetting;




LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (WM_PAINT == msg)
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		DoubleBuffer_Paint(&g_doubleBuffer, hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	else if (WM_MOUSEMOVE == msg)
	{

	}
	else if (WM_LBUTTONDOWN == msg)
	{

	}
	else if (WM_LBUTTONUP == msg)
	{

	}
	else if (WM_TIMER == msg)
	{

	}
	else if (WM_COMMAND == msg)
	{
		unsigned short id = LOWORD(wParam);
		if (id == IDC_BUTTON_CONN_PORT)
		{
			//MessageBox(g_hWnd, L"RS485 연결하기", L"", MB_OK);
			// 연결중이 아니라면
			memset(g_comName, 0, sizeof(g_comName));
			wchar_t comNumber[4] = { 0 };
			GetWindowText(g_hEditPortNumber, comNumber, sizeof(comNumber) / sizeof(comNumber[0]));
			int portNum = _wtoi(comNumber);
			sprintf(g_comName, "\\\\.\\COM%d", portNum);

			int nResult = openSerialPort(g_comName, 460800);
			if (nResult == 1)
			{
				// 연결 성공 시 내부에 현재 모드 요청 타이머 생성 (측정모드는 동작없음)
				// 연결 성공 시 내부에 현재 설정값 요청 타이머 생성 (측정모드는 동작없음)
				request_CurrentMode();
			}
		}
		else if (id == IDC_BUTTON_MODE_MEASUREMENT)
		{
			//MessageBox(g_hWnd, L"측정모드", L"", MB_OK);
			request_MeasurementMode();
		}
		else if (id == IDC_BUTTON_MODE_CONFIGURATION)
		{
			//MessageBox(g_hWnd, L"설정모드", L"", MB_OK);
			request_ConfigMode();
		}
		else if (id == IDC_BUTTON_SETTING_CHANGE)
		{
			//MessageBox(g_hWnd, L"설정변경", L"", MB_OK);
			ST_SETRAWDATACONFIG_50003 stData;
			initConfigData(&stData);
			stData.D10_11_number_distance_values = (unsigned short)end_spot;
			stData.D12_13_starting_spot = (unsigned short)start_spot;
			stData.D16_apd_distance_range = (unsigned char)apd_distance;

			// 값을 보내서 기존 측정값에 채워진 부분만 대입하여 보낸다
			request_changeSetting(stData);
			request_changeSetting(stData);
			request_changeSetting(stData);
		}
		else if (id == IDC_BUTTON_SETTING_RESET)
		{
			//MessageBox(g_hWnd, L"공장초기화", L"", MB_OK);
			request_RestoreSetting();
		}
		else if (id == IDC_BUTTON_SETTING_SAVE)
		{
			//MessageBox(g_hWnd, L"설정영구저장", L"", MB_OK);
			request_saveConfig_EEPROM();
		}
		return 0;
	}
	else if (WM_SIZE == msg)
	{
		DoubleBuffer_Resize(&g_doubleBuffer, hWnd, LOWORD(lParam), HIWORD(lParam));
	}
	else if (WM_NCHITTEST == msg)
	{
		// 마우스 위치에 따라 이동 가능하게
		LRESULT hitTest = DefWindowProc(hWnd, msg, wParam, lParam);
		if (hitTest == HTCLIENT) {
			return HTCAPTION;
		}
	}
	else if (WM_CLOSE == msg)
	{
		int nResult = MessageBoxW(hWnd, L"프로그램을 종료하시겠습니까?", L"종료 확인", MB_OKCANCEL | MB_ICONQUESTION);
		if (nResult == IDOK)
		{
			DestroyWindow(hWnd);
		}
		return 0;
	}
	else if (WM_DESTROY == msg)
	{
		DoubleBuffer_Destroy(&g_doubleBuffer);
		PostQuitMessage(0);
	}
	else if (WM_CREATE == msg)
	{
		DoubleBuffer_init(&g_doubleBuffer, hWnd);

		RECT rect;
		GetClientRect(hWnd, &rect);
		HRGN hRgn = CreateRoundRectRgn(
			rect.left, rect.top, rect.right, rect.bottom,
			40, 40
		);
		SetWindowRgn(hWnd, hRgn, TRUE);

		initPos();
		createControls(hWnd);

		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	g_hInst = hInstance;

	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"LZR_WIN";
	wcex.hIconSm = NULL;
	RegisterClassExW(&wcex);

	// 창의 크기를 지정
	RECT rc = { 0, 0, 800, 600 };               // 4:3
	::AdjustWindowRect(&rc, WS_POPUP | WS_THICKFRAME, FALSE);              // 여백보정

	g_wndWidth = rc.right - rc.left;
	g_wndHeight = rc.bottom - rc.top;

	g_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	g_screenHeight = GetSystemMetrics(SM_CYSCREEN);
	// 중앙 정렬 위치 계산
	g_posX = (g_screenWidth - g_wndWidth) / 2;
	g_posY = (g_screenHeight - g_wndHeight) / 2;

	g_hWnd = ::CreateWindowW(wcex.lpszClassName, L"LZR_WIN",
		WS_POPUP,
		g_posX, g_posY, g_wndWidth, g_wndHeight,
		NULL, NULL, g_hInst, NULL);

	g_dpi_response = 96.f / (float)GetDpiForSystem();

	ShowWindow(g_hWnd, true);
	UpdateWindow(g_hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LZRWIN));

	// 위치 설정
	

	// 큐에서 꺼내서 분기문 처리하는 쓰레드 생성
	

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}



	return (int)msg.wParam;
}



void ViewRgn_SetRgn(ST_ViewRgn* stRgn, int _sx, int _sy, int _ex, int _ey)
{
	stRgn->rect.left = _sx;
	stRgn->rect.top = _sy;
	stRgn->rect.right = _ex;
	stRgn->rect.bottom = _ey;
	if (_ex - _sx > 0) {
		stRgn->width = _ex - _sx;
	}
	else {
		stRgn->width = 0;
	}
	if (_ey - _sy > 0) {
		stRgn->height = _ey - _sy;
	}
	else {
		stRgn->height = 0;
	}
}

void createControls(HWND hWnd)
{
	// 컨트롤 생성
	g_hEditPortNumber = CreateWindowExW(
		WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_NUMBER,
		g_rgnEditPortNumber.rect.left, g_rgnEditPortNumber.rect.top, g_rgnEditPortNumber.width, g_rgnEditPortNumber.height,
		hWnd, (HMENU)IDC_EDIT_PORT_NUMBER,
		GetModuleHandle(NULL), NULL
	);
	SendMessageW(g_hEditPortNumber, EM_LIMITTEXT, (WPARAM)2, 0);				// 글자수 2글자 제한

	g_hBtnConnPort = CreateWindowW(
		L"BUTTON", L"연결",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		g_rgnBtnConnPort.rect.left, g_rgnBtnConnPort.rect.top, g_rgnBtnConnPort.width, g_rgnBtnConnPort.height,
		hWnd, (HMENU)IDC_BUTTON_CONN_PORT,
		GetModuleHandle(NULL), NULL
	);

	g_hBtnModeMeasurement = CreateWindowW(
		L"BUTTON", L"측정모드",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		g_rgnBtnModeMeasurement.rect.left, g_rgnBtnModeMeasurement.rect.top, g_rgnBtnModeMeasurement.width, g_rgnBtnModeMeasurement.height,
		hWnd, (HMENU)IDC_BUTTON_MODE_MEASUREMENT,
		GetModuleHandle(NULL), NULL
	);

	g_hBtnModeConfiguration = CreateWindowW(
		L"BUTTON", L"설정모드",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		g_rgnBtnModeConfiguration.rect.left, g_rgnBtnModeConfiguration.rect.top, g_rgnBtnModeConfiguration.width, g_rgnBtnModeConfiguration.height,
		hWnd, (HMENU)IDC_BUTTON_MODE_CONFIGURATION,
		GetModuleHandle(NULL), NULL
	);

	g_hEditStartSpot = CreateWindowExW(
		WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_NUMBER,
		g_rgnEditSetStartSpot.rect.left, g_rgnEditSetStartSpot.rect.top, g_rgnEditSetStartSpot.width, g_rgnEditSetStartSpot.height,
		hWnd, (HMENU)IDC_EDIT_START_SPOT,
		GetModuleHandle(NULL), NULL
	);
	SendMessageW(g_hEditStartSpot, EM_LIMITTEXT, (WPARAM)3, 0);				// 글자수 3글자 제한

	g_hEditEndSpot = CreateWindowExW(
		WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_NUMBER,
		g_rgnEditSetEndSpot.rect.left, g_rgnEditSetEndSpot.rect.top, g_rgnEditSetEndSpot.width, g_rgnEditSetEndSpot.height,
		hWnd, (HMENU)IDC_EDIT_END_SPOT,
		GetModuleHandle(NULL), NULL
	);

	// CBS_DROPDOWNLIST : 입력불가, CBS_DROPDOWN : 입력가능
	g_hComboDistance = CreateWindowExW(
		WS_EX_CLIENTEDGE, L"COMBOBOX", L"",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_AUTOHSCROLL | CBS_DROPDOWNLIST,
		g_rgnComboSetDistance.rect.left, g_rgnComboSetDistance.rect.top, g_rgnComboSetDistance.width, g_rgnComboSetDistance.height,
		hWnd, (HMENU)IDC_COMBO_DISTANCE,
		GetModuleHandle(NULL), NULL
	);
	//SendMessage(g_hComboDistance, WM_SETFONT, (WPARAM)g_hFont, TRUE);
	SendMessage(g_hComboDistance, CB_ADDSTRING, 0, (LPARAM)L"-");
	SendMessage(g_hComboDistance, CB_ADDSTRING, 0, (LPARAM)L"8m");
	SendMessage(g_hComboDistance, CB_ADDSTRING, 0, (LPARAM)L"12m");
	SendMessage(g_hComboDistance, CB_ADDSTRING, 0, (LPARAM)L"16m");
	SendMessage(g_hComboDistance, CB_SETCURSEL, 0, 0);			// 기본선택 0번

	g_hBtnChangeSetting = CreateWindowW(
		L"BUTTON", L"설정 변경",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		g_rgnBtnSetChangeSetting.rect.left, g_rgnBtnSetChangeSetting.rect.top, g_rgnBtnSetChangeSetting.width, g_rgnBtnSetChangeSetting.height,
		hWnd, (HMENU)IDC_BUTTON_SETTING_CHANGE,
		GetModuleHandle(NULL), NULL
	);
	g_hBtnInitSetting = CreateWindowW(
		L"BUTTON", L"초기 설정",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		g_rgnBtnSetInitSetting.rect.left, g_rgnBtnSetInitSetting.rect.top, g_rgnBtnSetInitSetting.width, g_rgnBtnSetInitSetting.height,
		hWnd, (HMENU)IDC_BUTTON_SETTING_RESET,
		GetModuleHandle(NULL), NULL
	);
	g_hBtnSaveSetting = CreateWindowW(
		L"BUTTON", L"설정 저장",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		g_rgnBtnSetSaveSetting.rect.left, g_rgnBtnSetSaveSetting.rect.top, g_rgnBtnSetSaveSetting.width, g_rgnBtnSetSaveSetting.height,
		hWnd, (HMENU)IDC_BUTTON_SETTING_SAVE,
		GetModuleHandle(NULL), NULL
	);
	
	g_isCreated = TRUE;
}

void initPos()
{
	// 위치 설정
	ViewRgn_SetRgn(&g_rgnTopBar, g_wndRect.left, g_wndRect.top, g_wndRect.right, calculateHeight(8));
	ViewRgn_SetRgn(&g_rgnTopBarTitle, g_wndRect.left, g_wndRect.top, g_wndRect.right, calculateHeight(8));
	ViewRgn_SetRgn(&g_rgnMDIViewer, calculateWidth(2), calculateHeight(10), calculateWidth(68), calculateHeight(70));

	ViewRgn_SetRgn(&g_rgnSettingViewerBox, calculateWidth(70), calculateHeight(10), calculateWidth(98), calculateHeight(70));
	ViewRgn_SetRgn(&g_rgnTextCurMode, calculateWidth(72), calculateHeight(12), calculateWidth(96), calculateHeight(16));
	//ViewRgn_SetRgn(&g_rgnTextCurModeValue, calculateWidth(72), calculateHeight(12), calculateWidth(96), calculateHeight(16));
	ViewRgn_SetRgn(&g_rgnTextBaudRate, calculateWidth(72), calculateHeight(18), calculateWidth(96), calculateHeight(22));
	ViewRgn_SetRgn(&g_rgnTextBaudRateValue, calculateWidth(72), calculateHeight(18), calculateWidth(96), calculateHeight(22));
	ViewRgn_SetRgn(&g_rgnTextStartingSpot, calculateWidth(72), calculateHeight(24), calculateWidth(96), calculateHeight(28));
	ViewRgn_SetRgn(&g_rgnTextStartingSpotValue, calculateWidth(72), calculateHeight(24), calculateWidth(96), calculateHeight(28));
	ViewRgn_SetRgn(&g_rgnTextStartingAngle, calculateWidth(72), calculateHeight(30), calculateWidth(96), calculateHeight(34));
	ViewRgn_SetRgn(&g_rgnTextStartingAngleValue, calculateWidth(72), calculateHeight(30), calculateWidth(96), calculateHeight(34));
	ViewRgn_SetRgn(&g_rgnTextEndSpot, calculateWidth(72), calculateHeight(36), calculateWidth(96), calculateHeight(40));
	ViewRgn_SetRgn(&g_rgnTextEndSpotValue, calculateWidth(72), calculateHeight(36), calculateWidth(96), calculateHeight(40));
	ViewRgn_SetRgn(&g_rgnTextEndAngle, calculateWidth(72), calculateHeight(42), calculateWidth(96), calculateHeight(46));
	ViewRgn_SetRgn(&g_rgnTextEndAngleValue, calculateWidth(72), calculateHeight(42), calculateWidth(96), calculateHeight(46));
	ViewRgn_SetRgn(&g_rgnAPD_Distance, calculateWidth(72), calculateHeight(48), calculateWidth(96), calculateHeight(52));
	ViewRgn_SetRgn(&g_rgnAPD_DistanceValue, calculateWidth(72), calculateHeight(48), calculateWidth(96), calculateHeight(52));
	ViewRgn_SetRgn(&g_rgnTextPlane0, calculateWidth(72), calculateHeight(54), calculateWidth(96), calculateHeight(58));
	ViewRgn_SetRgn(&g_rgnTextPlane0Value, calculateWidth(72), calculateHeight(54), calculateWidth(96), calculateHeight(58));
	ViewRgn_SetRgn(&g_rgnTextPlane1, calculateWidth(72), calculateHeight(60), calculateWidth(96), calculateHeight(64));
	ViewRgn_SetRgn(&g_rgnTextPlane1Value, calculateWidth(72), calculateHeight(60), calculateWidth(96), calculateHeight(64));
	ViewRgn_SetRgn(&g_rgnTextPlane2, calculateWidth(72), calculateHeight(66), calculateWidth(96), calculateHeight(70));
	ViewRgn_SetRgn(&g_rgnTextPlane2Value, calculateWidth(72), calculateHeight(66), calculateWidth(96), calculateHeight(70));
	ViewRgn_SetRgn(&g_rgnTextPlane3, calculateWidth(72), calculateHeight(72), calculateWidth(96), calculateHeight(76));
	ViewRgn_SetRgn(&g_rgnTextPlane3Value, calculateWidth(72), calculateHeight(72), calculateWidth(96), calculateHeight(76));

	ViewRgn_SetRgn(&g_rgnConnModeBox, calculateWidth(2), calculateHeight(72), calculateWidth(36), calculateHeight(98));
	ViewRgn_SetRgn(&g_rgnTextPortNumber, calculateWidth(4), calculateHeight(74), calculateWidth(10), calculateHeight(78));
	ViewRgn_SetRgn(&g_rgnEditPortNumber, calculateWidth(12), calculateHeight(74), calculateWidth(20), calculateHeight(78));
	ViewRgn_SetRgn(&g_rgnBtnConnPort, calculateWidth(28), calculateHeight(74), calculateWidth(34), calculateHeight(78));
	ViewRgn_SetRgn(&g_rgnBtnModeMeasurement, calculateWidth(4), calculateHeight(86), calculateWidth(18), calculateHeight(96));
	ViewRgn_SetRgn(&g_rgnBtnModeConfiguration, calculateWidth(20), calculateHeight(86), calculateWidth(34), calculateHeight(96));

	ViewRgn_SetRgn(&g_rgnSettingChangeBox, calculateWidth(38), calculateHeight(72), calculateWidth(98), calculateHeight(98));
	ViewRgn_SetRgn(&g_rgnTextSetStartSpot, calculateWidth(40), calculateHeight(74), calculateWidth(67), calculateHeight(78));
	ViewRgn_SetRgn(&g_rgnEditSetStartSpot, calculateWidth(40), calculateHeight(80), calculateWidth(67), calculateHeight(84));
	ViewRgn_SetRgn(&g_rgnTextSetEndSpot, calculateWidth(69), calculateHeight(74), calculateWidth(96), calculateHeight(78));
	ViewRgn_SetRgn(&g_rgnEditSetEndSpot, calculateWidth(69), calculateHeight(80), calculateWidth(96), calculateHeight(84));
	ViewRgn_SetRgn(&g_rgnTextSetDistance, calculateWidth(40), calculateHeight(86), calculateWidth(67), calculateHeight(90));
	ViewRgn_SetRgn(&g_rgnComboSetDistance, calculateWidth(69), calculateHeight(86), calculateWidth(96), calculateHeight(90));
	ViewRgn_SetRgn(&g_rgnBtnSetInitSetting, calculateWidth(40), calculateHeight(92), calculateWidth(57), calculateHeight(96));
	ViewRgn_SetRgn(&g_rgnBtnSetChangeSetting, calculateWidth(59.5), calculateHeight(92), calculateWidth(76.5), calculateHeight(96));
	ViewRgn_SetRgn(&g_rgnBtnSetSaveSetting, calculateWidth(79), calculateHeight(92), calculateWidth(96), calculateHeight(96));


}

void DoubleBuffer_Paint(ST_DoubleBuffer* pBuffer, HWND hWnd, PAINTSTRUCT* ps)
{
	HDC hdc = ps->hdc;
	
	HFONT hFont = CreateFontW(
		-24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, L"맑은 고딕"
	);
	HFONT hOldFont = (HFONT)SelectObject(pBuffer->m_hMemDC, hFont);
	SetTextColor(pBuffer->m_hMemDC, RGB(255, 255, 255));
	SetBkMode(pBuffer->m_hMemDC, TRANSPARENT);
	HBRUSH hBkBrush = CreateSolidBrush(RGB(3, 3, 3));
	FillRect(pBuffer->m_hMemDC, &ps->rcPaint, hBkBrush);

	if (g_isCreated == TRUE)
	{

		FillRect(pBuffer->m_hMemDC, &g_rgnTopBar.rect, (HBRUSH)GetStockObject(DKGRAY_BRUSH));

		HBRUSH hRectBrush = CreateSolidBrush(RGB(50, 50, 50));  // 회색 채우기
		HBRUSH hOldBrush = (HBRUSH)SelectObject(pBuffer->m_hMemDC, hRectBrush);

		HPEN hNullPen = (HPEN)GetStockObject(NULL_PEN);  // 테두리 없음
		HPEN hOldPen = (HPEN)SelectObject(pBuffer->m_hMemDC, hNullPen);
		RoundRect(pBuffer->m_hMemDC, g_rgnMDIViewer.rect.left, g_rgnMDIViewer.rect.top, g_rgnMDIViewer.rect.right, g_rgnMDIViewer.rect.bottom, 20, 20);
		RoundRect(pBuffer->m_hMemDC, g_rgnSettingViewerBox.rect.left, g_rgnSettingViewerBox.rect.top, g_rgnSettingViewerBox.rect.right, g_rgnSettingViewerBox.rect.bottom, 20, 20);
		RoundRect(pBuffer->m_hMemDC, g_rgnConnModeBox.rect.left, g_rgnConnModeBox.rect.top, g_rgnConnModeBox.rect.right, g_rgnConnModeBox.rect.bottom, 20, 20);
		RoundRect(pBuffer->m_hMemDC, g_rgnSettingChangeBox.rect.left, g_rgnSettingChangeBox.rect.top, g_rgnSettingChangeBox.rect.right, g_rgnSettingChangeBox.rect.bottom, 20, 20);

		DrawText(pBuffer->m_hMemDC, g_szTitle, -1, &g_rgnTopBarTitle.rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		DrawText(pBuffer->m_hMemDC, g_szCurMode, -1, &g_rgnTextCurMode.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szBaudRate, -1, &g_rgnTextBaudRate.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szStartingSpot, -1, &g_rgnTextStartingSpot.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szStartingSpotAngle, -1, &g_rgnTextStartingAngle.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szEndSpot, -1, &g_rgnTextEndSpot.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szEndSpotAngle, -1, &g_rgnTextEndAngle.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szAPDDistance, -1, &g_rgnAPD_Distance.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		//DrawText(pBuffer->m_hMemDC, g_szPlane0, -1, &g_rgnTextPlane0.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		//DrawText(pBuffer->m_hMemDC, g_szPlane1, -1, &g_rgnTextPlane1.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		//DrawText(pBuffer->m_hMemDC, g_szPlane2, -1, &g_rgnTextPlane2.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		//DrawText(pBuffer->m_hMemDC, g_szPlane3, -1, &g_rgnTextPlane3.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		//DrawText(pBuffer->m_hMemDC, x, -1, &g_rgnTextCurModeValue.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szBaudRateValue, -1, &g_rgnTextBaudRateValue.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szStartingSpotValue, -1, &g_rgnTextStartingSpotValue.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szStartingSpotAngleValue, -1, &g_rgnTextStartingAngleValue.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szEndSpotValue, -1, &g_rgnTextEndSpotValue.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szEndSpotAngleValue, -1, &g_rgnTextEndAngleValue.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szAPDDistanceValue, -1, &g_rgnAPD_DistanceValue.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		//DrawText(pBuffer->m_hMemDC, g_szPlane0Value, -1, &g_rgnTextPlane0Value.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		//DrawText(pBuffer->m_hMemDC, g_szPlane1Value, -1, &g_rgnTextPlane1Value.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		//DrawText(pBuffer->m_hMemDC, g_szPlane2Value, -1, &g_rgnTextPlane2Value.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
		//DrawText(pBuffer->m_hMemDC, g_szPlane3Value, -1, &g_rgnTextPlane3Value.rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

		DrawText(pBuffer->m_hMemDC, g_szPort, -1, &g_rgnTextPortNumber.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		DrawText(pBuffer->m_hMemDC, g_szSetStartSpot, -1, &g_rgnTextSetStartSpot.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szSetEndSpot, -1, &g_rgnTextSetEndSpot.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		DrawText(pBuffer->m_hMemDC, g_szSetDistance, -1, &g_rgnTextSetDistance.rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		// 복원 및 정리
		SelectObject(pBuffer->m_hMemDC, hOldBrush);
		SelectObject(pBuffer->m_hMemDC, hOldPen);
		DeleteObject(hRectBrush);
	}

	DeleteObject(hBkBrush);
	SelectObject(pBuffer->m_hMemDC, hOldFont);
	DeleteObject(hFont);  // 리소스 해제
	// 화면에 복사
	BitBlt(hdc, 0, 0, pBuffer->m_clientWidth, pBuffer->m_clientHeight, pBuffer->m_hMemDC, 0, 0, SRCCOPY);
}

void DoubleBuffer_init(ST_DoubleBuffer* pBuffer, HWND hWnd)
{
	GetClientRect(hWnd, &g_wndRect);

	pBuffer->m_clientWidth = g_wndRect.right - g_wndRect.left;
	pBuffer->m_clientHeight = g_wndRect.bottom - g_wndRect.top;

	HDC hdc = GetDC(hWnd);
	pBuffer->m_hMemDC = CreateCompatibleDC(hdc);
	pBuffer->m_hBitmap = CreateCompatibleBitmap(hdc, pBuffer->m_clientWidth, pBuffer->m_clientHeight);
	pBuffer->m_hOldBitmap = (HBITMAP)SelectObject(pBuffer->m_hMemDC, pBuffer->m_hBitmap);
	ReleaseDC(hWnd, hdc);
}

void DoubleBuffer_Resize(ST_DoubleBuffer* pBuffer, HWND hWnd, int newWidth, int newHeight)
{
	if (newWidth > 0 && newHeight > 0)
	{
		if (pBuffer->m_hBitmap)
		{
			SelectObject(pBuffer->m_hMemDC, pBuffer->m_hOldBitmap);
			DeleteObject(pBuffer->m_hBitmap);
		}

		HDC hdc = GetDC(hWnd);
		pBuffer->m_hBitmap = CreateCompatibleBitmap(hdc, newWidth, newHeight);
		pBuffer->m_hOldBitmap = (HBITMAP)SelectObject(pBuffer->m_hMemDC, pBuffer->m_hBitmap);
		ReleaseDC(hWnd, hdc);

		pBuffer->m_clientWidth = newWidth;
		pBuffer->m_clientHeight = newHeight;
	}
}

void DoubleBuffer_Destroy(ST_DoubleBuffer* pBuffer)
{
	if (pBuffer->m_hMemDC)
	{
		SelectObject(pBuffer->m_hMemDC, pBuffer->m_hOldBitmap);
		DeleteObject(pBuffer->m_hBitmap);
		DeleteDC(pBuffer->m_hMemDC);

		pBuffer->m_hMemDC = NULL;
		pBuffer->m_hBitmap = NULL;
		pBuffer->m_hOldBitmap = NULL;
		pBuffer->m_clientWidth = 0;
		pBuffer->m_clientHeight = 0;
	}
}

float calculateWidth(float fPercentage)
{
	return ((static_cast<float>(g_wndWidth) * fPercentage) / 100.0f);
}
float calculateHeight(float fPercentage)
{
	return ((static_cast<float>(g_wndHeight) * fPercentage) / 100.0f);
}