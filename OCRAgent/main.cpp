#include "framework.h"
#include "main.h"

HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
HWND g_hWnd;

class CMain
{
public:
	float m_dpi_response;
	int m_wndWidth;
	int m_wndHeight;
	int m_screenWidth;
	int m_screenHeight;
	int m_posX;
	int m_posY;
	BOOL m_isCreated;
	BOOL m_program_end;

	CMain()
	{
		m_dpi_response = 0.0f;
		m_wndWidth = 0;
		m_wndHeight = 0;
		m_screenWidth = 0;
		m_screenHeight = 0;
		m_posX = 0;
		m_posY = 0;
		m_isCreated = FALSE;
		m_program_end = FALSE;
	}
	void checkFolder()
	{

	}
	bool loadINI(const std::string filePath)
	{
		namespace fs = std::filesystem;

		// Check if the file exists
		if (!fs::exists(filePath)) {
			MessageBoxA(NULL, "INI file not found!", "Error", MB_OK | MB_ICONERROR);
			return false;
		}

		// Open the file
		std::ifstream iniFile(filePath);
		if (!iniFile.is_open()) {
			MessageBoxA(NULL, "Failed to open INI file!", "Error", MB_OK | MB_ICONERROR);
			return false;
		}

		// Sample parsing logic (assuming key=value pairs)
		std::string line;
		while (std::getline(iniFile, line)) {
			// Skip empty lines or comments
			if (line.empty() || line[0] == '#') {
				continue;
			}

			size_t delimiterPos = line.find('=');
			if (delimiterPos != std::string::npos) {
				std::string key = line.substr(0, delimiterPos);
				std::string value = line.substr(delimiterPos + 1);

				// Example: Print the key-value pair
				std::cout << "Key: " << key << ", Value: " << value << std::endl;
			}
		}

		iniFile.close();
		return true;
	}
};

CMain* g_pMain = NULL;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (WM_PAINT == msg)
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);


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
		if (wParam == ID_TIMER_CHECK_FOLDER)
		{
			g_pMain->checkFolder();
			//MessageBox(g_hWnd, L"테스트용", L"test", MB_OK);
		}
	}
	else if (WM_COMMAND == msg)
	{
		// 버튼 이벤트 ID_BTN
		unsigned short id = LOWORD(wParam);

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
		PostQuitMessage(0);
	}
	else if (WM_CREATE == msg)
	{

		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	g_hInst = hInstance;
	g_pMain = new CMain;

	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"ocragent";
	wcex.hIconSm = NULL;
	RegisterClassExW(&wcex);

	// ini 파일을 불러와서 셋팅한다
	g_pMain->loadINI("./ocr.ini");

	// 창의 크기를 지정
	RECT rc = { 0, 0, 800, 600};

	g_pMain->m_wndWidth = rc.right - rc.left;
	g_pMain->m_wndHeight = rc.bottom - rc.top;

	g_pMain->m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	g_pMain->m_screenHeight = GetSystemMetrics(SM_CYSCREEN);
	// 중앙 정렬 위치 계산
	g_pMain->m_posX = (g_pMain->m_screenWidth - g_pMain->m_wndWidth) / 2;
	g_pMain->m_posY = (g_pMain->m_screenHeight - g_pMain->m_wndHeight) / 2;

	g_hWnd = ::CreateWindowW(wcex.lpszClassName, L"OCR Agent",
		WS_EX_CLIENTEDGE,
		g_pMain->m_posX, g_pMain->m_posY, g_pMain->m_wndWidth, g_pMain->m_wndHeight,
		NULL, NULL, g_hInst, NULL);

	g_pMain->m_dpi_response = 96.f / (float)GetDpiForSystem();

	ShowWindow(g_hWnd, true);
	UpdateWindow(g_hWnd);

	// 폴더 체크 타이머 시작
	SetTimer(g_hWnd, ID_TIMER_CHECK_FOLDER, 10000, NULL);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OCRAGENT));

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 리소스 정리
	KillTimer(g_hWnd, ID_TIMER_CHECK_FOLDER);
	delete g_pMain;

	return (int)msg.wParam;
}
