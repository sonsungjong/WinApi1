
#include "pch.h"
#include "main.h"

#include <LogBufferModule/LogBufferModule.h>
#include "LZR920_Define.h"
#include "LZR920_Enum.h"
//#include "LZR920_Struct.h"
#include "MainController.h"


#ifdef _DEBUG
#pragma comment(lib, "LogBufferModule//LogBufferModuleD.lib")
#else
#pragma comment(lib, "LogBufferModule//LogBufferModule.lib")
#endif

HINSTANCE g_hInst;
HWND g_hWnd;
int g_wndWidth;
int g_wndHeight;
volatile int configure_flag = 0;
BOOL g_nCount = 0;
std::map<unsigned char, std::vector<unsigned short>> g_mapMDI;
std::mutex m_mtxMDI;

HANDLE g_hSerial;
std::vector<POINT> g_points;
constexpr int PLANE_COLOR = 4;
//constexpr int POINT_COUNT = 274;

// 각 Plane 색상
COLORREF PLANE_COLORS[PLANE_COLOR] = {
    RGB(255, 255, 255),     // Plane 0 - 빨강
    RGB(255, 212, 0),     // Plane 1 - 노랑
    RGB(0, 255, 0),     // Plane 2 - 초록
    RGB(238, 130, 238)    // Plane 3 - 바이올렛
};

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
    wcex.hbrBackground = CreateSolidBrush(RGB(13, 13, 20));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"LZR920Class";
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    // 초기화
    if (!_openSerialPort()) {
        MessageBox(NULL, _T("시리얼 포트 초기화 실패"), _T("ERROR"), MB_ICONERROR);
        return -1;
    }

    // 영역 계산
    RECT rcClient = { 0, 0, 1540, 800};
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
                PreTranslateMessage(&msg);
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
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_TAB) {
            
        }
        else if (pMsg->wParam == VK_RETURN) {
            printf("Enter");
        }
        else if (pMsg->wParam == VK_F1) {
            sendEnterMeasureMode();
        }
        else if (pMsg->wParam == VK_F2) {
            SetTimer(g_hWnd, 2, 1000, NULL);
        }
        else if (pMsg->wParam == VK_F3) {
            printf("F3");
        }
        else if (pMsg->wParam == VK_F4) {

        }
        else if (pMsg->wParam == VK_F5) {

        }
        else if (pMsg->wParam == VK_F6) {

        }
        else if (pMsg->wParam == VK_F7) {

        }
        else if (pMsg->wParam == VK_F8) {

        }
        else if (pMsg->wParam == VK_F9) {

        }
        else if (pMsg->wParam == VK_F10) {

        }
        else if (pMsg->wParam == VK_F11) {

        }
        else if (pMsg->wParam == VK_F12) {

        }
    }
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
    std::thread(_recvSerialPort).detach();

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
    if (id == 1)
    {
        
    }
    else if (id == 2)
    {
        if (configure_flag == 1 || g_nCount > 10)
        {
            KillTimer(hWnd, 2);
            configure_flag = 0;
            g_nCount = 0;
        }
        sendEnterConfigMode();
        g_nCount++;
    }
}

// "\\\\.\\COM9"
// 460800
bool _openSerialPort(const char* portName, unsigned int baudRate)
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

void _recvSerialPort()
{
    const int buf_size = BUFFER_SIZE;
    unsigned char* buf = new unsigned char[buf_size] {};
    DWORD bytesRead;

    std::vector<unsigned char> dataBuffer;

    while (true)
    {
        memset(buf, 0, buf_size);
        if (ReadFile(g_hSerial, buf, buf_size, &bytesRead, NULL))
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
                        if (cmd == 50011)               // MDI
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

                                    {
                                        std::lock_guard<std::mutex> lg(m_mtxMDI);
                                        g_mapMDI[planeNumber] = mdi;
                                    }

                                }
                                RECT rt{ 0, 0, 1920, 1080 };
                                InvalidateRect(g_hWnd, &rt, TRUE);
                                //for (const auto& plane_mdi : mapMDI)
                                //{
                                //    std::string strPlane = "===========" + std::to_string(plane_mdi.first) + "===========";
                                //    //CLogBufferModule::getInstance().log(strPlane);
                                //    std::string strMDI = "\n";
                                //    for (const unsigned short& mdi_data : plane_mdi.second)
                                //    {
                                //        strMDI = strMDI + std::to_string(mdi_data) + "\n";
                                //    }
                                //    //CLogBufferModule::getInstance().log(strMDI);
                                //}

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
                        else if (cmd == 50002)          // 모드 변경 응답
                        {
                            unsigned char mode = data[0];               // [1] 측정모드 [2] 설정모드
                            if (mode == 1)
                            {
                                printf("측정모드\n");
                            }
                            else if(mode == 2)
                            {
                                configure_flag = 1;
                                printf("설정모드\n");
                            }
                            else {
                                printf("알수없는 모드\n");
                            }
                        }
                        else 
                        {
                            data;
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


//void DrawGDI(HDC hdc)
//{
//    std::lock_guard<std::mutex> lg(m_mtxMDI);
//
//    RECT rc;
//    GetClientRect(g_hWnd, &rc);
//    int wndWidth = rc.right - rc.left;
//    int wndHeight = rc.bottom - rc.top;
//
//    int pxCenter = wndWidth / 2;
//    int pyCenter = wndHeight / 2;
//
//    constexpr double scale = 10.0; // 거리 스케일 조정
//
//    for (int p = 0; p < PLANE_COUNT; ++p) {
//        COLORREF color = PLANE_COLORS[p];
//
//        if (g_mapMDI.count(p) == 0 || g_mapMDI[p].size() != POINT_COUNT)
//            continue;
//
//        for (int i = 0; i < POINT_COUNT; ++i) {
//            double angle_deg = -48.0 + i * (96.0 / (POINT_COUNT - 1));
//            double angle_rad = angle_deg * 3.141592 / 180.0;
//            double distance = static_cast<double>(g_mapMDI[p][i]);
//
//            double x = distance * cos(angle_rad);
//            double y = distance * sin(angle_rad);
//
//            int px = static_cast<int>(pxCenter + x / scale);
//            int py = static_cast<int>(pyCenter - y / scale); // y축 반전
//
//            SetPixel(hdc, px, py, color);
//        }
//    }
//}

void DrawGDI(HDC hdc)
{
    std::lock_guard<std::mutex> lg(m_mtxMDI);

    RECT rc;
    GetClientRect(g_hWnd, &rc);
    int wndWidth = rc.right - rc.left;
    int wndHeight = rc.bottom - rc.top;

    int pxCenter = wndWidth / 2;
    int pyCenter = wndHeight / 2;

    constexpr double scale = 10.0;

    //for (int p = 0; p < g_mapMDI.size(); ++p)
    for (int p = 0; p < 1; ++p)                 // 테스트용
    {
        COLORREF color = PLANE_COLORS[p];

        int point_count = g_mapMDI[p].size();

        // 동일 색상이므로, 펜과 브러시를 한 번만 생성
        HPEN hPen = CreatePen(PS_SOLID, 1, color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hBrush = CreateSolidBrush(color);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        for (int i = 0; i < point_count; ++i) {
            int ellipse_radius = 1;
            double angle_deg = -48.0 + i * (96.0 / (point_count - 1));
            double angle_rad = (angle_deg) * 3.141592 / 180.0;
            double distance = static_cast<double>(g_mapMDI[p][i]);

            double x = distance * cos(angle_rad);
            double y = distance * sin(angle_rad);

            int px = static_cast<int>(pxCenter + x / scale);
            int py = static_cast<int>(pyCenter - y / scale);

            Ellipse(hdc, px - ellipse_radius, py - ellipse_radius, px + ellipse_radius, py + ellipse_radius);
        }


        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
    }
}


// 설정 모드로
void sendEnterConfigMode()
{
    std::thread([]() {
        unsigned char flag = 0xA5;
        DWORD bytesWritten = 0;
        WriteFile(g_hSerial, &flag, 1, &bytesWritten, NULL);

        if (bytesWritten == 1) {
            CLogBufferModule::getInstance().log("설정 모드 진입 명령 전송 완료 (0xA5)");
        }
        else {
            CLogBufferModule::getInstance().log("설정 모드 진입 명령 전송 실패");
        }
    }).detach();
}

// 측정 모드로
void sendEnterMeasureMode()
{
    // 프레임 구성: SYNC(4) + SIZE(2) + CMD(2) + DATA(1) + CHK(2) = 11 bytes
    std::thread([]() {
        unsigned char packet[11] = {};
        unsigned int sync = HEADER_SYNC_VAL;
        unsigned short size = 3;             // CMD(2) + DATA(1)
        unsigned short cmd = 50001;         // SETRAWDATAMODE
        unsigned char d0 = 1;             // 측정 모드 요청

        // CHK 계산
        unsigned short chk = 0;
        chk += cmd & 0xFF;        // LSB
        chk += (cmd >> 8);        // MSB
        chk += d0;

        // 패킷 작성 (LSB first)
        memcpy(&packet[0], &sync, 4);           // SYNC
        memcpy(&packet[4], &size, 2);           // SIZE
        memcpy(&packet[6], &cmd, 2);            // CMD
        packet[8] = d0;                         // DATA
        memcpy(&packet[9], &chk, 2);            // CHK

        DWORD bytesWritten = 0;
        WriteFile(g_hSerial, packet, sizeof(packet), &bytesWritten, NULL);

        if (bytesWritten == sizeof(packet)) {
            CLogBufferModule::getInstance().log("측정 모드 진입 명령 전송 완료 (0x5001)");
        }
        else {
            CLogBufferModule::getInstance().log("측정 모드 진입 명령 전송 실패");
        }
    }).detach();
}
