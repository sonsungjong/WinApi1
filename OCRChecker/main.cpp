#include "framework.h"
#include "main.h"

HINSTANCE g_hInst;                                // 현재 인스턴스입니다.
HWND g_hWnd;

static std::wstring Utf8ToWide(const std::string& str)
{
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

static std::string WideToUtf8(const std::wstring& wstr)
{
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

static void TrimInPlace(std::wstring& s)
{
    auto is_ws = [](wchar_t ch){ return ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n'; };
    size_t a = 0; while (a < s.size() && is_ws(s[a])) ++a;
    size_t b = s.size(); while (b > a && is_ws(s[b-1])) --b;
    if (a > 0 || b < s.size()) s = s.substr(a, b - a);
}

struct AppConfig
{
    std::wstring checkFolder; // CHECK_FOLDER PATH
    std::wstring imageFolder; // IMAGE_FOLDER PATH
    std::wstring ocrServer;   // OCR_SERVER URL e.g. localhost:8000
};

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

    // UI
    HWND m_btnStart{};
    HWND m_btnCancel{};
    HWND m_status{};

    // State
    AppConfig m_cfg{};
    std::atomic<bool> m_processing{ false };
    std::atomic<bool> m_cancelRequested{ false };
    std::thread m_worker;
    std::mutex m_mu;

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

    ~CMain()
    {
        RequestCancel();
        if (m_worker.joinable()) m_worker.join();
    }

    void SetStatus(const std::wstring& text)
    {
        if (m_status)
            SetWindowTextW(m_status, text.c_str());
    }

    static bool ReadIniString(const wchar_t* file, const wchar_t* section, const wchar_t* key, std::wstring& out)
    {
        wchar_t buf[4096]{};
        DWORD read = GetPrivateProfileStringW(section, key, L"", buf, 4096, file);
        if (read == 0) return false;
        out.assign(buf, read);
        return true;
    }

    bool loadINI(const std::wstring& filePath)
    {
        // default to exe directory when relative path provided
        WCHAR exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        std::filesystem::path p(exePath);
        std::filesystem::path ini = filePath;
        if (!ini.is_absolute()) ini = p.parent_path() / ini;

        std::wstring check, image, url;
        bool ok = true;
        ok &= ReadIniString(ini.c_str(), L"CHECK_FOLDER", L"PATH", check);
        ok &= ReadIniString(ini.c_str(), L"IMAGE_FOLDER", L"PATH", image);
        ok &= ReadIniString(ini.c_str(), L"OCR_SERVER", L"URL", url);

        if (!ok)
        {
            MessageBoxW(NULL, L"INI 파일을 읽을 수 없습니다.", L"오류", MB_OK | MB_ICONERROR);
            return false;
        }

        TrimInPlace(check);
        TrimInPlace(image);
        TrimInPlace(url);

        m_cfg.checkFolder = check;
        m_cfg.imageFolder = image;
        m_cfg.ocrServer = url;
        return true;
    }

    // Ensure both folders exist (create if missing)
    void EnsureFolders()
    {
        namespace fs = std::filesystem;
        try {
            if (!m_cfg.checkFolder.empty()) fs::create_directories(m_cfg.checkFolder);
            if (!m_cfg.imageFolder.empty()) fs::create_directories(m_cfg.imageFolder);
        } catch (...) {}
    }

    static bool IsImageFileExt(const std::wstring& ext)
    {
        std::wstring e = ext;
        for (auto& ch : e) ch = (wchar_t)towupper(ch);
        return (e == L".PNG" || e == L".JPG" || e == L".JPEG");
    }

    static std::wstring GetNameRoot(const std::wstring& filename)
    {
        // split at ^#### pattern; return name before ^ if exists
        size_t pos = filename.find(L'^');
        if (pos != std::wstring::npos)
            return filename.substr(0, pos);
        std::wstring stem = std::filesystem::path(filename).stem().wstring();
        return stem;
    }

    static std::string ReadFileToBase64(const std::wstring& path)
    {
        std::ifstream ifs(path, std::ios::binary);
        if (!ifs) return {};
        std::vector<unsigned char> data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        if (data.empty()) return {};
        return Base64Encode(data.data(), data.size());
    }

    static std::string Base64Encode(const unsigned char* data, size_t len)
    {
        static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string out;
        out.reserve(((len + 2) / 3) * 4);
        size_t i = 0;
        while (i + 2 < len) {
            unsigned triple = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
            out.push_back(b64_table[(triple >> 18) & 0x3F]);
            out.push_back(b64_table[(triple >> 12) & 0x3F]);
            out.push_back(b64_table[(triple >> 6) & 0x3F]);
            out.push_back(b64_table[triple & 0x3F]);
            i += 3;
        }
        if (i < len) {
            unsigned triple = data[i] << 16;
            if (i + 1 < len) triple |= data[i + 1] << 8;
            out.push_back(b64_table[(triple >> 18) & 0x3F]);
            out.push_back(b64_table[(triple >> 12) & 0x3F]);
            if (i + 1 < len) {
                out.push_back(b64_table[(triple >> 6) & 0x3F]);
                out.push_back('=');
            }
            else {
                out.push_back('=');
                out.push_back('=');
            }
        }
        return out;
    }

    static bool HttpPostJson(const std::wstring& hostAndPort, const std::wstring& path, const std::string& json, std::string& outResponse, DWORD timeoutMs)
    {
        // hostAndPort format: host:port or host
        std::wstring host = hostAndPort;
        INTERNET_PORT port = 80;
        size_t colon = hostAndPort.find(L':');
        if (colon != std::wstring::npos) {
            host = hostAndPort.substr(0, colon);
            port = (INTERNET_PORT)_wtoi(hostAndPort.substr(colon + 1).c_str());
            if (port == 0) port = 80;
        }

        HINTERNET hSession = WinHttpOpen(L"OCRChecker/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) {
            #ifdef _DEBUG
            OutputDebugStringA("WinHttpOpen 실패\n");
            #endif
            return false;
        }
        WinHttpSetTimeouts(hSession, timeoutMs, timeoutMs, timeoutMs, timeoutMs);

        HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
        if (!hConnect) { 
            #ifdef _DEBUG
            OutputDebugStringA("WinHttpConnect 실패\n");
            #endif
            WinHttpCloseHandle(hSession); 
            return false; 
        }

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        if (!hRequest) { 
            #ifdef _DEBUG
            OutputDebugStringA("WinHttpOpenRequest 실패\n");
            #endif
            WinHttpCloseHandle(hConnect); 
            WinHttpCloseHandle(hSession); 
            return false; 
        }

        const wchar_t* headers = L"Content-Type: application/json; charset=utf-8\r\n";
        BOOL b = WinHttpSendRequest(hRequest, headers, (DWORD)-1L, (LPVOID)json.data(), (DWORD)json.size(), (DWORD)json.size(), 0);
        if (!b) { 
            #ifdef _DEBUG
            OutputDebugStringA("WinHttpSendRequest 실패\n");
            #endif
            WinHttpCloseHandle(hRequest); 
            WinHttpCloseHandle(hConnect); 
            WinHttpCloseHandle(hSession); 
            return false; 
        }
        b = WinHttpReceiveResponse(hRequest, NULL);
        if (!b) { 
            #ifdef _DEBUG
            OutputDebugStringA("WinHttpReceiveResponse 실패\n");
            #endif
            WinHttpCloseHandle(hRequest); 
            WinHttpCloseHandle(hConnect); 
            WinHttpCloseHandle(hSession); 
            return false; 
        }

        DWORD dwSize = 0;
        outResponse.clear();
        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (dwSize == 0) break;
            std::string buffer;
            buffer.resize(dwSize);
            DWORD dwDownloaded = 0;
            if (!WinHttpReadData(hRequest, &buffer[0], dwSize, &dwDownloaded)) break;
            buffer.resize(dwDownloaded);
            outResponse += buffer;
        } while (dwSize > 0);

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return true;
    }

    void StartTimer()
    {
        SetTimer(g_hWnd, ID_TIMER_CHECK_FOLDER, 5000, NULL);
        SetStatus(L"감시중");
    }

    void StopTimer()
    {
        BOOL result = KillTimer(g_hWnd, ID_TIMER_CHECK_FOLDER);
        if (!result) {
            // KillTimer 실패 시 로그 (디버그용)
            #ifdef _DEBUG
            OutputDebugStringA("KillTimer failed\n");
            #endif
        }
    }

    void RequestCancel()
    {
        m_cancelRequested.store(true);
    }

    void EnsureWorkerJoined()
    {
        if (m_worker.joinable()) m_worker.join();
    }

    void checkFolder()
    {
        if (m_processing.load()) return; // already processing
        
        namespace fs = std::filesystem;
        fs::path checkDir = m_cfg.checkFolder;
        if (!fs::exists(checkDir)) {
            std::error_code ec; fs::create_directories(checkDir, ec);
            if (!fs::exists(checkDir)) return;
        }
        fs::path flag = checkDir / L"ocr.txt";
        if (!fs::exists(flag)) return;

        // stop timer and start processing
        StopTimer();
        m_processing.store(true);
        m_cancelRequested.store(false);
        SetStatus(L"처리중");

        // 기존 스레드가 있으면 정리
        if (m_worker.joinable()) {
            m_worker.join();
        }

        // start worker
        m_worker = std::thread([this]() { this->ProcessImagesLoop(); });
    }

    void ForceStart()
    {
        checkFolder();
    }

    void ForceCancel()
    {
        // 이미 처리 중이 아니면 무시
        if (!m_processing.load()) {
            return;
        }

        // 취소 요청 설정
        m_cancelRequested.store(true);
        
        // 서버에 취소 요청 전송
        SendCancelToServer();
        
        // 플래그 파일 삭제
        DeleteFlagFile();
        
        // 기존 스레드가 완료될 때까지 대기
        EnsureWorkerJoined();
        
        // 처리 상태 초기화
        m_processing.store(false);
        
        // 상태를 감시중으로 변경
        SetStatus(L"감시중");
        
        // 타이머 재시작
        StartTimer();
    }

    void DeleteFlagFile()
    {
        try {
            std::filesystem::path flag = std::filesystem::path(m_cfg.checkFolder) / L"ocr.txt";
            if (std::filesystem::exists(flag)) {
                bool removed = std::filesystem::remove(flag);
                if (!removed) {
                    // 플래그 파일 삭제 실패 로그 (디버그용)
                    #ifdef _DEBUG
                    OutputDebugStringA("ocr.txt 파일 삭제 실패\n");
                    #endif
                }
            }
        }
        catch (...) {}
    }

    void AppendLinesToExcelCsvCompat(const std::vector<std::string>& lines)
    {
        std::filesystem::path outPath = std::filesystem::path(m_cfg.checkFolder) / L"활동데이터_견적서템플릿.xlsx";
        try { std::filesystem::create_directories(outPath.parent_path()); } catch (...) {}
        std::ofstream ofs(outPath, std::ios::binary | std::ios::app);
        if (!ofs) return;
        for (auto& l : lines) {
            ofs.write(l.data(), l.size());
            ofs.write("\r\n", 2);
        }
    }

    void ProcessImagesLoop()
    {
        namespace fs = std::filesystem;
        // 이미지 폴더에서 이미지를 찾음 (INI 설정의 IMAGE_FOLDER 사용)
        fs::path imgDir = m_cfg.imageFolder;
        if (!fs::exists(imgDir)) {
            std::error_code ec; fs::create_directories(imgDir, ec);
            if (!fs::exists(imgDir)) {
                SetStatus(L"이미지 폴더가 존재하지 않습니다");
                m_processing.store(false);
                StartTimer();
                return;
            }
        }

        bool anyProcessed = false;
        bool errorOccurred = false;

        while (!m_cancelRequested.load())
        {
            // collect all images, sorted
            std::vector<fs::path> all;
            for (auto& entry : fs::directory_iterator(imgDir)) {
                if (!entry.is_regular_file()) continue;
                fs::path p = entry.path();
                std::wstring ext = p.extension().wstring();
                if (!IsImageFileExt(ext)) continue;
                all.push_back(p);
            }
            std::sort(all.begin(), all.end());

            // 디버그: 찾은 이미지 파일 수 출력
            #ifdef _DEBUG
            std::wstring debugMsg = L"찾은 이미지 파일 수: " + std::to_wstring(all.size()) + L"개\n";
            OutputDebugStringW(debugMsg.c_str());
            for (const auto& img : all) {
                std::wstring fileMsg = L"  - " + img.filename().wstring() + L"\n";
                OutputDebugStringW(fileMsg.c_str());
            }
            #endif

            if (all.empty()) {
                // 이미지가 없으면 플래그 파일만 삭제하고 종료
                DeleteFlagFile();
                SetStatus(L"처리할 이미지가 없습니다");
                break;
            }

            // build a group for the initial root
            std::wstring name0 = all.front().filename().wstring();
            std::wstring root = GetNameRoot(name0);
            std::vector<fs::path> files;
            for (auto& p : all) {
                if (GetNameRoot(p.filename().wstring()) == root) {
                    files.push_back(p);
                }
            }

            // build JSON payload for this group
            std::vector<std::string> items;
            for (auto& p : files)
            {
                if (m_cancelRequested.load()) break;
                std::string b64 = ReadFileToBase64(p.wstring());
                if (b64.empty()) {
                    #ifdef _DEBUG
                    std::wstring errorMsg = L"파일 읽기 실패: " + p.filename().wstring() + L"\n";
                    OutputDebugStringW(errorMsg.c_str());
                    #endif
                    continue;
                }
                std::wstring wfname = p.filename().wstring();
                std::string fname = WideToUtf8(wfname);
                std::string item = "{\"filename\":\"" + EscapeJson(fname) + "\",\"data\":\"" + EscapeJson(b64) + "\"}";
                items.push_back(std::move(item));
            }

            if (items.empty()) {
                if (m_cancelRequested.load()) break;
                errorOccurred = true;
                SetStatus(L"이미지 읽기 실패");
                break;
            }

            std::string json;
            json.reserve(16 + items.size() * 64);
            json.push_back('[');
            for (size_t i = 0; i < items.size(); ++i)
            {
                if (i > 0) json.push_back(',');
                json += items[i];
            }
            json.push_back(']');

            if (m_cancelRequested.load()) break;

            SetStatus(L"서버에 전송 중...");
            
            #ifdef _DEBUG
            debugMsg = L"전송할 JSON 크기: " + std::to_wstring(json.size()) + L" bytes\n";
            OutputDebugStringW(debugMsg.c_str());
            #endif
            
            std::string resp;
            bool ok = HttpPostJson(m_cfg.ocrServer, L"/ocr", json, resp, 10 * 60 * 1000);
            
            #ifdef _DEBUG
            std::wstring serverMsg = L"서버 통신 결과: " + std::wstring(ok ? L"성공" : L"실패") + L"\n";
            OutputDebugStringW(serverMsg.c_str());
            #endif
            
            if (!ok) {
                SetStatus(L"서버 통신 실패");
                errorOccurred = true;
                break;
            }
            
            #ifdef _DEBUG
            std::wstring respMsg = L"서버 응답 크기: " + std::to_wstring(resp.size()) + L" bytes\n";
            OutputDebugStringW(respMsg.c_str());
            if (resp.size() > 0) {
                std::wstring respContent = L"서버 응답 내용: " + std::wstring(resp.begin(), resp.end()) + L"\n";
                OutputDebugStringW(respContent.c_str());
            }
            #endif
            
            SetStatus(L"서버 응답 받음, 파일 삭제 중...");

            AppendLinesToExcelCsvCompat({ resp });

            // delete processed files
            #ifdef _DEBUG
            std::wstring deleteMsg = L"삭제할 파일 수: " + std::to_wstring(files.size()) + L"개\n";
            OutputDebugStringW(deleteMsg.c_str());
            #endif
            
            for (auto& p : files) {
                std::error_code ec; 
                bool removed = fs::remove(p, ec);
                if (removed) {
                    #ifdef _DEBUG
                    std::wstring successMsg = L"파일 삭제 성공: " + p.filename().wstring() + L"\n";
                    OutputDebugStringW(successMsg.c_str());
                    #endif
                } else {
                    // 파일 삭제 실패 로그 (디버그용)
                    #ifdef _DEBUG
                    std::wstring msg = L"파일 삭제 실패: " + p.wstring() + L" (에러코드: " + std::to_wstring(ec.value()) + L")\n";
                    OutputDebugStringW(msg.c_str());
                    #endif
                }
            }
            anyProcessed = true;
        }

        #ifdef _DEBUG
        std::wstring finalMsg = L"처리 완료 - anyProcessed: " + std::wstring(anyProcessed ? L"true" : L"false") + 
                               L", errorOccurred: " + std::wstring(errorOccurred ? L"true" : L"false") + 
                               L", cancelRequested: " + std::wstring(m_cancelRequested.load() ? L"true" : L"false") + L"\n";
        OutputDebugStringW(finalMsg.c_str());
        #endif

        if (!m_cancelRequested.load() && !errorOccurred && anyProcessed) {
            #ifdef _DEBUG
            OutputDebugStringA("플래그 파일 삭제 시도\n");
            #endif
            DeleteFlagFile();
            SetStatus(L"감시중");
        } else if (errorOccurred) {
            SetStatus(L"오류 발생");
        } else if (m_cancelRequested.load()) {
            SetStatus(L"취소됨");
        } else {
            SetStatus(L"처리할 이미지 없음");
        }
        
        #ifdef _DEBUG
        OutputDebugStringA("처리 상태 초기화 및 타이머 재시작\n");
        #endif
        m_processing.store(false);
        StartTimer();
    }

    static std::string EscapeJson(const std::string& s)
    {
        std::string out; out.reserve(s.size());
        for (char c : s) {
            switch (c) {
            case '\\': out += "\\\\"; break;
            case '"': out += "\\\""; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if ((unsigned char)c < 0x20) {
                    char buf[7];
                    sprintf_s(buf, "\\u%04x", (unsigned char)c);
                    out += buf;
                }
                else out += c;
            }
        }
        return out;
    }

    void SendCancelToServer()
    {
        // optional: POST {"cancel":true}
        std::string resp;
        const char* body = "{\"cancel\":true}";
        HttpPostJson(m_cfg.ocrServer, L"/cancel", body, resp, 10 * 1000);
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
        if (id == ID_BTN_FORCE_START) {
            g_pMain->ForceStart();
        }
        else if (id == ID_BTN_FORCE_CANCEL) {
            g_pMain->ForceCancel();
        }
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
        // Create buttons and status text
        g_pMain->m_btnStart = CreateWindowW(L"BUTTON", L"강제시작", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            20, 20, 120, 30, hWnd, (HMENU)ID_BTN_FORCE_START, g_hInst, NULL);
        g_pMain->m_btnCancel = CreateWindowW(L"BUTTON", L"강제취소", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            160, 20, 120, 30, hWnd, (HMENU)ID_BTN_FORCE_CANCEL, g_hInst, NULL);
        g_pMain->m_status = CreateWindowW(L"STATIC", L"감시중", WS_VISIBLE | WS_CHILD,
            20, 60, 400, 20, hWnd, NULL, g_hInst, NULL);

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
    wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"ocrchecker";
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    // ini 파일을 불러와서 셋팅한다
    g_pMain->loadINI(L"./OCRChecker.ini");
    // 폴더 없으면 생성
    g_pMain->EnsureFolders();

    // 창의 크기를 지정
    RECT rc = { 0, 0, 400, 200 };

    g_pMain->m_wndWidth = rc.right - rc.left;
    g_pMain->m_wndHeight = rc.bottom - rc.top;

    g_pMain->m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
    g_pMain->m_screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // 중앙 정렬 위치 계산
    g_pMain->m_posX = (g_pMain->m_screenWidth - g_pMain->m_wndWidth) / 2;
    g_pMain->m_posY = (g_pMain->m_screenHeight - g_pMain->m_wndHeight) / 2;

    g_hWnd = ::CreateWindowW(wcex.lpszClassName, L"OCR Checker",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        g_pMain->m_posX, g_pMain->m_posY, g_pMain->m_wndWidth, g_pMain->m_wndHeight,
        NULL, NULL, g_hInst, NULL);

    g_pMain->m_dpi_response = 96.f / (float)GetDpiForSystem();

    ShowWindow(g_hWnd, true);
    UpdateWindow(g_hWnd);

    // 폴더 체크 타이머 시작
    SetTimer(g_hWnd, ID_TIMER_CHECK_FOLDER, 5000, NULL);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OCRCHECKER));

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
