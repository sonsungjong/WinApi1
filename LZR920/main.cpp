#include "pch.h"
#include "main.h"
#include <LogBufferModule/LogBufferModule.h>
#include "serial_rs485.h"


#ifdef _DEBUG
#pragma comment(lib, "LogBufferModule//LogBufferModuleD.lib")
#else
#pragma comment(lib, "LogBufferModule//LogBufferModule.lib")
#endif

HINSTANCE g_hInst;
HWND g_hWnd;
int g_wndWidth;
int g_wndHeight;

HANDLE g_hSerial;
std::vector<POINT> g_points;
std::mutex g_mtx;
std::vector<unsigned char> g_vecBuf;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    g_hInst = hInstance;

    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(243, 243, 243));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"LZR920Class";
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    // 초기화
    if (!openSerialPort()) {
        MessageBox(NULL, _T("시리얼 포트 초기화 실패"), _T("ERROR"), MB_ICONERROR);
        return -1;
    }

    // 영역 계산
    RECT rcClient = { 0, 0, 800, 800 };
    AdjustWindowRect(&rcClient, WS_POPUP | WS_THICKFRAME, FALSE);

    g_hWnd = ::CreateWindowW(wcex.lpszClassName, L"LZR920Class",
        WS_POPUP | WS_THICKFRAME,
        rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
        NULL, NULL, hInstance, NULL);

    RECT rc_wnd;
    if (GetClientRect(g_hWnd, &rc_wnd)) {
        g_wndWidth = rc_wnd.right - rc_wnd.left;
        g_wndHeight = rc_wnd.bottom - rc_wnd.top;
    }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LZR920));

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        // DX는 항상 렌더링 될 수 있게 PeekMessage를 사용해야한다
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 메시지 처리
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                //PreTranslateMessage(&msg);
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // 렌더링
            
        }
    }

    return (int)msg.wParam;
}

BOOL PreTranslateMessage(MSG* pMsg)
{

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (WM_PAINT == msg)
    {
        OnPaint(hWnd);
    }
    else if (WM_NCHITTEST == msg)
    {
        // 창을 드래그할 수 있게 변경
        return HTCAPTION;
    }
    else if (WM_COMMAND == msg)
    {
        int id = LOWORD(wParam);                        // 명령ID
        HWND hwndCtrl = (HWND)lParam;           // 명령을 발생시킨 컨트롤 핸들 (없으면 NULL)
        UINT codeNotify = HIWORD(wParam);       // 알림 코드
        OnCommand(hWnd, id, hwndCtrl, codeNotify);
    }
    else if (WM_TIMER == msg)
    {
        UINT_PTR timerID = (UINT_PTR)wParam;                    // 타이머 식별자
        OnTimer(hWnd, timerID);
    }
    else if (WM_CLOSE == msg)
    {
        OnClose(hWnd);
    }
    else if (WM_DESTROY == msg)
    {
        OnDestroy(hWnd);
    }
    else if (WM_CREATE == msg)
    {
        OnCreate(hWnd, (LPCREATESTRUCT)lParam);
    }
    else
    {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    DrawGDI(hdc);

    EndPaint(hWnd, &ps);
}

int OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
    std::thread(recvSerialPort).detach();

    return 0;
}

void OnDestroy(HWND hWnd)
{
    // 리소스 정리


    PostQuitMessage(0);
}

void OnClose(HWND hWnd)
{
    int nResult = MessageBoxW(hWnd, L"프로그램을 종료하시겠습니까?", L"종료 확인", MB_OKCANCEL | MB_ICONQUESTION);
    if (nResult == IDOK)
    {
        DestroyWindow(hWnd);
    }
}

void OnCommand(HWND hWnd, int id, HWND hwndCtrl, UINT codeNotify)
{
    switch (id)
    {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
    }
}

void OnTimer(HWND hWnd, UINT_PTR id)
{

}

// "\\\\.\\COM9"
// 460800
bool openSerialPort(const char* portName, unsigned int baudRate)
{
    bool bResult = false;
    g_hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (g_hSerial != INVALID_HANDLE_VALUE) {
        DCB dcb = { 0 };
        dcb.DCBlength = sizeof(DCB);
        BOOL state = GetCommState(g_hSerial, &dcb);
        if (TRUE == state)
        {
            dcb.BaudRate = baudRate;
            dcb.ByteSize = 8;
            dcb.Parity = NOPARITY;
            dcb.StopBits = ONESTOPBIT;
            SetCommState(g_hSerial, &dcb);

            // 타임아웃 설정
            COMMTIMEOUTS timeouts = { 10, 1, 10, 0, 0 };
            SetCommTimeouts(g_hSerial, &timeouts);

            bResult = true;
        }
    }

    return bResult;
}

void recvSerialPort()
{
    const int BUFFER_SIZE = 65535;
    unsigned char* buf = new unsigned char[BUFFER_SIZE] {};
    DWORD bytesRead;

    std::vector<unsigned char> dataBuffer;

    while (true)
    {
        memset(buf, 0, BUFFER_SIZE);
        if (ReadFile(g_hSerial, buf, BUFFER_SIZE, &bytesRead, NULL)) 
        {
            CLogBufferModule::getInstance().log("==>수신크기: " + std::to_string(bytesRead));

            dataBuffer.insert(dataBuffer.end(), buf, buf + bytesRead);

            while (dataBuffer.size() >= 6)  // 최소 헤더
            {
                // SYNC 검사 (FC FD FE FF)
                unsigned int sync = 0U;
                memcpy(&sync, dataBuffer.data(), sizeof(unsigned int));

                if (dataBuffer.size() < 8) {
                    break;
                }

                if (sync == 0xFFFEFDFC)             // SYNC 확인
                {
                    // SIZE 읽기 (LSB first)
                    unsigned short size = 0U;
                    memcpy(&size, dataBuffer.data() + 4, sizeof(unsigned short));
                    if (dataBuffer.size() < 6 + size + 2) {
                        break; // 아직 전체 메시지가 안 들어왔으니 기다려야 함
                    }

                    // 메시지 추출
                    std::vector<unsigned char> message(dataBuffer.begin() + 6, dataBuffer.begin() + 6 + size);
                    unsigned short footer_chk = 0U;
                    memcpy(&footer_chk, dataBuffer.data() + 6 + size, sizeof(unsigned short));

                    // 여기서 payload부터가 CMD, 그 뒤가 DATA, 마지막 2바이트는 CHK
                    unsigned short cmd = 0U;
                    memcpy(&cmd, message.data(), sizeof(unsigned short));
                    std::vector<unsigned char> data(message.begin() + 2, message.begin() + size);

                    // CHK 검증
                    unsigned short calc_chk = 0;
                    for (unsigned char b : message)
                    {
                        calc_chk += b;
                    }

                    footer_chk = dataBuffer[6 + size] | (dataBuffer[6 + size + 1] << 8);
                    if (calc_chk == footer_chk)
                    {
                        printf("TRUE");
                        if (cmd == 50011)
                        {
                            if (data.size() == 2202)
                            {
                                // ID + Frame counter
                                // Plane Number + MDI

                                // 앞에 6바이트를 다시 떼어내고
                                // 그 다음 Plane Number 1바이트를 읽고
                                // 그 다음 548 바이트를 unsigned short로 각각 변환한다음
                                // 다시 Plane Number 1바이트를 읽고
                                // 다시 548 바이트를 unsigned short로 각각 변환하고... 총 4번
                                std::vector<unsigned char> id_frame_counter(data.begin(), data.begin() + 6);
                                std::map<unsigned char, std::vector<unsigned short>> mapMDI;
                                std::vector<unsigned char> vecPlane;

                                size_t idx = 6;
                                const int planeCount = 4;
                                for (int count = 0; count < planeCount; ++count)
                                {
                                    // 1바이트를 읽어서 Plane Number 를 키로 넣는다
                                    unsigned char planeNumber = data[idx];
                                    vecPlane.push_back(planeNumber);
                                    idx++;

                                    std::vector<unsigned short> mdi;
                                    mdi.reserve(274);
                                    for (int i = 0; i < 274; ++i) {
                                        // 두 바이트를 조합한다
                                        // little-endian
                                        //unsigned short value = data[idx] | (data[idx + 1] << 8);
                                        unsigned short value = 0U;
                                        memcpy(&value, &data[idx], sizeof(unsigned short));
                                        mdi.push_back(value);
                                        idx += 2;
                                    }

                                    mapMDI[planeNumber] = mdi;
                                }

                                for (const auto& plane_mdi : mapMDI)
                                {
                                    std::string strPlane = "===========" + std::to_string(plane_mdi.first) + "===========";
                                    //CLogBufferModule::getInstance().log(strPlane);
                                    std::string strMDI = "\n";
                                    for (const unsigned short& mdi_data : plane_mdi.second)
                                    {
                                        strMDI = strMDI + std::to_string(mdi_data) + "\n";
                                    }
                                    //CLogBufferModule::getInstance().log(strMDI);
                                }

                            }
                            else if(data.size() == 2196)
                            {
                                // Plane Number + MDI

                            }
                            else if (data.size() == 2216)
                            {
                                // ID + Frame counter
                                // CTN + VNR + Error log + Hot reset counter
                                // Plane Number + MDI

                            }
                            else if (data.size() == 2210)
                            {
                                // CTN + VNR + Error log + Hot reset counter
                                // Plane Number + MDI

                            }
                        }

                        // 데이터
                        printf("[INFO] SIZE: %u, CMD: 0x%04X\n", size, cmd);
                    }
                    else {
                        printf("FALSE");
                    }

                    // 처리 끝났으니 제거
                    dataBuffer.erase(dataBuffer.begin(), dataBuffer.begin() + size);
                }
                else {
                    // SYNC 틀리면 한 바이트씩 버림
                    dataBuffer.erase(dataBuffer.begin());
                }
            }
        }
        else {
            printf("[ERROR] ReadFile 실패\n");
            break;
        }
        
    }

    delete[] buf;
}

void parseMDIMessage(const unsigned char* data, size_t size)
{
    if (size < 274 || data[0] != 0x02 || data[1] != 'M' || data[2] != 'D' || data[3] != 'I')
        return;

    std::lock_guard<std::mutex> lock(g_mtx);
    g_points.clear();

    // 271개 포인트, 각 포인트 2바이트 거리 데이터 (Plane 1 기준)
    int centerX = g_wndWidth / 2;
    int centerY = g_wndHeight / 2;
    float angle_step = 275.0f / 270.0f; // 약 1도 간격 (275포인트)

    for (int i = 0; i < 271; ++i)
    {
        int offset = 4 + i * 2; // MDI 시작 후 4바이트부터 거리
        uint16_t distance = (data[offset] << 8) | data[offset + 1]; // big endian

        if (distance == 0 || distance > 30000) continue; // 거리 0 or 너무 크면 무시

        float angle_deg = -135.0f + i * angle_step; // -135도 ~ +135도
        float angle_rad = angle_deg * 3.14159265f / 180.0f;

        int x = centerX + static_cast<int>((distance / 100.0f) * cos(angle_rad));
        int y = centerY - static_cast<int>((distance / 100.0f) * sin(angle_rad));

        g_points.push_back(POINT{ x, y });
    }

    InvalidateRect(g_hWnd, NULL, TRUE); // 그리기 요청
}


void DrawGDI(HDC hdc)
{
    std::lock_guard<std::mutex> lock(g_mtx);
    for (const auto& pt : g_points) {
        Ellipse(hdc, pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
    }
}
