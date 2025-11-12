#include "framework.h"
#include "main.h"

#include <nlohmann/json.hpp>
#include <xlnt/xlnt.hpp>

#ifdef _DEBUG
#pragma comment(lib, "xlntd.lib")
#else
#pragma comment(lib, "xlnt.lib")
#endif

HINSTANCE g_hInst;
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
    std::wstring ocrServer;   // OCR_SERVER URL
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
        // Create ocr.txt in CHECK_FOLDER
        namespace fs = std::filesystem;
        fs::path checkDir = m_cfg.checkFolder;
        
        // Ensure folder exists
        if (!fs::exists(checkDir)) {
            std::error_code ec;
            fs::create_directories(checkDir, ec);
        }
        
        // Create ocr.txt file
        fs::path flagFile = checkDir / L"ocr.txt";
        try {
            std::ofstream ofs(flagFile);
            if (ofs) {
                ofs << "start" << std::endl;
                ofs.close();
                
                #ifdef _DEBUG
                OutputDebugStringA("ocr.txt 파일 생성 완료\n");
                #endif
            }
        } catch (...) {
            #ifdef _DEBUG
            OutputDebugStringA("ocr.txt 파일 생성 실패\n");
            #endif
        }
        
        // Then trigger normal processing
        checkFolder();
    }

    void ForceCancel()
    {
        // if not processing, ignore
        if (!m_processing.load()) {
            return;
        }

        // set cancel request
        m_cancelRequested.store(true);
        
        // send cancel request to server
        SendCancelToServer();
        
        // delete flag file
        DeleteFlagFile();
        
        // wait for existing thread to complete
        EnsureWorkerJoined();
        
        // initialize processing state
        m_processing.store(false);
        
        // change status to monitoring
        SetStatus(L"감시중");
        
        // restart timer
        StartTimer();
    }

    void DeleteFlagFile()
    {
        try {
            std::filesystem::path flag = std::filesystem::path(m_cfg.checkFolder) / L"ocr.txt";
            if (std::filesystem::exists(flag)) {
                bool removed = std::filesystem::remove(flag);
                if (!removed) {
                    #ifdef _DEBUG
                    OutputDebugStringA("ocr.txt delete failed\n");
                    #endif
                }
            }
        }
        catch (...) {}
    }

    // 파일을 CHECK_FOLDER로 이동하는 함수
    void MoveFilesToCheckFolder(const std::vector<std::filesystem::path>& files)
    {
        namespace fs = std::filesystem;
        fs::path checkDir = m_cfg.checkFolder;
        
        // CHECK_FOLDER가 존재하는지 확인
        if (!fs::exists(checkDir)) {
            std::error_code ec;
            fs::create_directories(checkDir, ec);
        }
        
        int successCount = 0;
        int failCount = 0;
        
        for (auto& srcPath : files) {
            try {
                // 대상 경로: CHECK_FOLDER / 파일명
                fs::path destPath = checkDir / srcPath.filename();
                
                // 같은 이름의 파일이 이미 존재하면 타임스탬프 추가
                if (fs::exists(destPath)) {
                    auto now = std::chrono::system_clock::now();
                    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                        now.time_since_epoch()).count();
                    
                    std::wstring stem = destPath.stem().wstring();
                    std::wstring ext = destPath.extension().wstring();
                    std::wstring newName = stem + L"_" + std::to_wstring(timestamp) + ext;
                    destPath = checkDir / newName;
                }
                
                // 파일 이동 (rename 사용)
                std::error_code ec;
                fs::rename(srcPath, destPath, ec);
                
                if (!ec) {
                    successCount++;
                    #ifdef _DEBUG
                    std::wstring msg = L"파일 이동 성공: " + srcPath.filename().wstring() + 
                                      L" -> " + destPath.wstring() + L"\n";
                    OutputDebugStringW(msg.c_str());
                    #endif
                } else {
                    // rename 실패 시 copy + remove 시도
                    fs::copy(srcPath, destPath, fs::copy_options::overwrite_existing, ec);
                    if (!ec) {
                        fs::remove(srcPath, ec);
                        if (!ec) {
                            successCount++;
                            #ifdef _DEBUG
                            std::wstring msg = L"파일 복사+삭제 성공: " + srcPath.filename().wstring() + 
                                              L" -> " + destPath.wstring() + L"\n";
                            OutputDebugStringW(msg.c_str());
                            #endif
                        } else {
                            failCount++;
                            #ifdef _DEBUG
                            std::wstring msg = L"파일 삭제 실패: " + srcPath.wstring() + 
                                              L" (에러: " + std::to_wstring(ec.value()) + L")\n";
                            OutputDebugStringW(msg.c_str());
                            #endif
                        }
                    } else {
                        failCount++;
                        #ifdef _DEBUG
                        std::wstring msg = L"파일 이동 실패: " + srcPath.filename().wstring() + 
                                          L" (에러: " + std::to_wstring(ec.value()) + L")\n";
                        OutputDebugStringW(msg.c_str());
                        #endif
                    }
                }
            } catch (const std::exception& e) {
                failCount++;
                #ifdef _DEBUG
                std::string msg = "파일 이동 예외: ";
                msg += e.what();
                msg += "\n";
                OutputDebugStringA(msg.c_str());
                #endif
            }
        }
        
        #ifdef _DEBUG
        wchar_t buf[256];
        swprintf_s(buf, L"파일 이동 완료: 성공 %d개, 실패 %d개\n", successCount, failCount);
        OutputDebugStringW(buf);
        #endif
    }

    void AppendLinesToExcelCsvCompat(const std::vector<std::string>& lines)
    {
        using json = nlohmann::json;
        
        std::filesystem::path outPath = std::filesystem::path(m_cfg.checkFolder) / L"활동데이터_견적서템플릿.xlsx";
        try { 
            std::filesystem::create_directories(outPath.parent_path()); 
        } catch (...) {}
        
        // 컬럼 헤더 정의
        const std::vector<std::wstring> headers = {
            L"품번", L"품목", L"품명", L"규격", L"수량", L"단가", L"최종금액", L"제조사", L"사이즈"
        };
        
        // JSON 키와 컬럼 인덱스 매핑 (새로운 키값으로 업데이트)
        const std::map<std::string, int> keyToColumn = {
            {"partNumber", 1},      // 품번
            {"itemCategory", 2},    // 품목
            {"productName", 3},     // 품명
            {"specifications", 4},  // 규격
            {"quantity", 5},        // 수량
            {"unitPrice", 6},       // 단가
            {"totalAmount", 7},     // 최종금액
            {"manufacturer", 8},    // 제조사
            {"size", 9}             // 사이즈
        };
        
        try {
            xlnt::workbook wb;
            xlnt::worksheet ws;
            bool isNewFile = false;
            
            // 파일이 이미 존재하면 로드, 없으면 새로 생성
            if (std::filesystem::exists(outPath)) {
                try {
                    wb.load(outPath.wstring());
                    ws = wb.active_sheet();
                } catch (const std::exception& e) {
                    #ifdef _DEBUG
                    std::string msg = "기존 파일 로드 실패, 새로 생성: ";
                    msg += e.what();
                    msg += "\n";
                    OutputDebugStringA(msg.c_str());
                    #endif
                    // 로드 실패 시 새 파일 생성
                    wb = xlnt::workbook();
                    ws = wb.active_sheet();
                    ws.title("OCR Results");
                    isNewFile = true;
                }
            } else {
                ws = wb.active_sheet();
                ws.title("OCR Results");
                isNewFile = true;
            }
            
            // 다음 행 번호 찾기 (xlnt는 column, row 순서!)
            int nextRow = 1;
            if (!isNewFile) {
                try {
                    while (ws.cell(1, nextRow).has_value()) {
                        nextRow++;
                    }
                } catch (...) {
                    // 빈 셀을 만나면 여기서 멈춤
                }
            }
            
            // 새 파일이거나 첫 행이 비어있으면 헤더 추가 (가로로!)
            if (isNewFile || nextRow == 1) {
                for (size_t i = 0; i < headers.size(); ++i) {
                    // UTF-8로 변환하여 1행의 각 열에 저장 (column=i+1, row=1)
                    std::string utf8Header = WideToUtf8(headers[i]);
                    ws.cell(static_cast<int>(i + 1), 1).value(utf8Header);
                }
                nextRow = 2;
            }
            
            // JSON 파싱해서 Excel에 기록
            for (auto& line : lines) {
                try {
                    // JSON 파싱
                    json j = json::parse(line);
                    
                    // 배열이면 각 요소를 한 줄씩
                    if (j.is_array()) {
                        for (auto& item : j) {
                            // 객체면 키-값 매핑으로 컬럼에 기록
                            if (item.is_object()) {
                                for (auto it = item.begin(); it != item.end(); ++it) {
                                    auto colIt = keyToColumn.find(it.key());
                                    if (colIt != keyToColumn.end()) {
                                        int col = colIt->second;
                                        
                                        // 값 추출 및 처리 (column, row 순서!)
                                        if (it.value().is_string()) {
                                            std::string value = it.value().get<std::string>();
                                            ws.cell(col, nextRow).value(value);
                                        } else if (it.value().is_number_integer()) {
                                            ws.cell(col, nextRow).value(it.value().get<int64_t>());
                                        } else if (it.value().is_number_float()) {
                                            ws.cell(col, nextRow).value(it.value().get<double>());
                                        } else {
                                            std::string value = it.value().dump();
                                            // 따옴표 제거
                                            if (!value.empty() && value.front() == '"' && value.back() == '"') {
                                                value = value.substr(1, value.length() - 2);
                                            }
                                            ws.cell(col, nextRow).value(value);
                                        }
                                    }
                                }
                                nextRow++;
                            } 
                            // 단순 값이면 첫 번째 컬럼에
                            else {
                                if (item.is_string()) {
                                    ws.cell(1, nextRow).value(item.get<std::string>());
                                } else {
                                    std::string value = item.dump();
                                    if (!value.empty() && value.front() == '"' && value.back() == '"') {
                                        value = value.substr(1, value.length() - 2);
                                    }
                                    ws.cell(1, nextRow).value(value);
                                }
                                nextRow++;
                            }
                        }
                    }
                    // 객체면 한 줄로
                    else if (j.is_object()) {
                        for (auto it = j.begin(); it != j.end(); ++it) {
                            auto colIt = keyToColumn.find(it.key());
                            if (colIt != keyToColumn.end()) {
                                int col = colIt->second;
                                
                                if (it.value().is_string()) {
                                    ws.cell(col, nextRow).value(it.value().get<std::string>());
                                } else if (it.value().is_number_integer()) {
                                    ws.cell(col, nextRow).value(it.value().get<int64_t>());
                                } else if (it.value().is_number_float()) {
                                    ws.cell(col, nextRow).value(it.value().get<double>());
                                } else {
                                    std::string value = it.value().dump();
                                    if (!value.empty() && value.front() == '"' && value.back() == '"') {
                                        value = value.substr(1, value.length() - 2);
                                    }
                                    ws.cell(col, nextRow).value(value);
                                }
                            }
                        }
                        nextRow++;
                    }
                    // 단순 값이면 그대로
                    else {
                        ws.cell(1, nextRow).value(line);
                        nextRow++;
                    }
                } catch (const json::parse_error& e) {
                    #ifdef _DEBUG
                    std::string errorMsg = "JSON 파싱 실패: ";
                    errorMsg += e.what();
                    errorMsg += "\n";
                    OutputDebugStringA(errorMsg.c_str());
                    #endif
                    // 파싱 실패 시 원본 텍스트 저장
                    ws.cell(1, nextRow).value(line);
                    nextRow++;
                }
            }
            
            wb.save(outPath.wstring());
            
            #ifdef _DEBUG
            OutputDebugStringA("Excel 파일 저장 완료\n");
            #endif
        } catch (const std::exception& e) {
            #ifdef _DEBUG
            std::string errorMsg = "Excel 저장 실패: ";
            errorMsg += e.what();
            errorMsg += "\n";
            OutputDebugStringA(errorMsg.c_str());
            #endif
        }
    }

    // JSON 응답 데이터 검증 함수 (품명은 필수이고 최종금액 또는 단가 중 하나가 있어야함 체크)
    static bool ValidateResponseData(const std::string& resp, std::wstring& errorDetails)
    {
        using json = nlohmann::json;
        
        if (resp.empty()) {
            errorDetails = L"서버 응답이 비어있습니다.";
            return false;
        }
        
        // 기본 JSON 검증
        if (!IsValidJson(resp)) {
            errorDetails = L"유효하지 않은 JSON 형식입니다.";
            return false;
        }
        
        try {
            json j = json::parse(resp);
            
            // 배열이 아니면 오류
            if (!j.is_array()) {
                errorDetails = L"응답이 배열 형식이 아닙니다.";
                return false;
            }
            
            // 빈 배열이면 오류
            if (j.empty()) {
                errorDetails = L"응답 데이터가 비어있습니다.";
                return false;
            }
            
            // 각 항목에 대해 필수 필드 확인
            int itemIndex = 0;
            for (auto& item : j) {
                itemIndex++;
                
                if (!item.is_object()) {
                    wchar_t buf[256];
                    swprintf_s(buf, L"항목 %d: 객체 형식이 아닙니다.", itemIndex);
                    errorDetails = buf;
                    return false;
                }
                
                // 필수 필드: productName (품명)
                if (!item.contains("productName") || !item["productName"].is_string() || 
                    item["productName"].get<std::string>().empty()) {
                    wchar_t buf[256];
                    swprintf_s(buf, L"항목 %d: 품명(productName)이 없거나 비어있습니다.", itemIndex);
                    errorDetails = buf;
                    return false;
                }
                
                // 필수 필드: totalAmount (최종금액) 또는 unitPrice (단가) 중 하나
                bool hasTotalAmount = item.contains("totalAmount") && 
                                     item["totalAmount"].is_string() && 
                                     !item["totalAmount"].get<std::string>().empty();
                
                bool hasUnitPrice = item.contains("unitPrice") && 
                                   item["unitPrice"].is_string() && 
                                   !item["unitPrice"].get<std::string>().empty();
                
                if (!hasTotalAmount && !hasUnitPrice) {
                    wchar_t buf[256];
                    swprintf_s(buf, L"항목 %d: 최종금액(totalAmount) 또는 단가(unitPrice) 중 하나가 필요합니다.", itemIndex);
                    errorDetails = buf;
                    return false;
                }
            }
            
            return true;
            
        } catch (const json::parse_error& e) {
            std::string errorMsg = "JSON 파싱 오류: ";
            errorMsg += e.what();
            errorDetails = Utf8ToWide(errorMsg);
            return false;
        } catch (const std::exception& e) {
            std::string errorMsg = "데이터 검증 오류: ";
            errorMsg += e.what();
            errorDetails = Utf8ToWide(errorMsg);
            return false;
        }
    }

    // JSON 유효성 검사 함수
    static bool IsValidJson(const std::string& str)
    {
        if (str.empty()) return false;
        
        // 기본적인 JSON 검증: 최소한 { } 또는 [ ]로 시작하고 끝나는지 확인
        size_t start = 0;
        size_t end = str.length();
        
        // 앞뒤 공백 제거
        while (start < end && (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n')) ++start;
        while (end > start && (str[end-1] == ' ' || str[end-1] == '\t' || str[end-1] == '\r' || str[end-1] == '\n')) --end;
        
        if (end <= start) return false;
        
        char first = str[start];
        char last = str[end-1];
        
        // JSON은 { }나 [ ]로 시작하고 끝나야 함
        if ((first == '{' && last == '}') || (first == '[' && last == ']')) {
            return true;
        }
        
        return false;
    }

    void ProcessImagesLoop()
    {
        namespace fs = std::filesystem;
        // IMAGE_FOLDER
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

            #ifdef _DEBUG
            std::wstring debugMsg = L"찾은 이미지 파일 수: " + std::to_wstring(all.size()) + L"개\n";
            OutputDebugStringW(debugMsg.c_str());
            for (const auto& img : all) {
                std::wstring fileMsg = L"  - " + img.filename().wstring() + L"\n";
                OutputDebugStringW(fileMsg.c_str());
            }
            #endif

            if (all.empty()) {
                // if no images, delete flag file and exit
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

            // ⭐ 정상 응답 플래그 초기화
            bool isValidResponse = false;

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
                // 서버 통신 실패 - 파일을 CHECK_FOLDER로 이동
                SetStatus(L"서버 통신 실패 - 파일 이동 중...");
                
                std::wstring errorMsg = L"서버 통신에 실패했습니다.\n\n";
                errorMsg += L"이미지 파일들을 OCR 폴더로 이동합니다.";
                MessageBoxW(g_hWnd, errorMsg.c_str(), L"서버 통신 오류", MB_OK | MB_ICONWARNING);
                
                MoveFilesToCheckFolder(files);
                errorOccurred = true;
                break;
            }
            
            // 서버 응답 데이터 검증 (품명, 최종금액 필수)
            std::wstring validationError;
            if (!ValidateResponseData(resp, validationError)) {
                // 응답 검증 실패 - 파일을 CHECK_FOLDER로 이동
                SetStatus(L"비정상 서버 응답 - 파일 이동 중...");
                
                #ifdef _DEBUG
                OutputDebugStringW((L"응답 검증 실패: " + validationError + L"\n").c_str());
                #endif
                
                // 비정상 응답이므로 파일을 CHECK_FOLDER로 이동
                MoveFilesToCheckFolder(files);
                errorOccurred = true;
                break;
            }
            
            // ⭐ 검증 통과 - 플래그 설정
            isValidResponse = true;
            
            #ifdef _DEBUG
            std::wstring respMsg = L"서버 응답 크기: " + std::to_wstring(resp.size()) + L" bytes\n";
            OutputDebugStringW(respMsg.c_str());
            if (resp.size() > 0) {
                std::wstring respContent = L"서버 응답 내용: " + Utf8ToWide(resp) + L"\n";
                OutputDebugStringW(respContent.c_str());
            }
            #endif
            
            SetStatus(L"서버 응답 받음, Excel 저장 중...");

            // 응답 검증을 통과했으므로 엑셀에 저장
            AppendLinesToExcelCsvCompat({ resp });

            // ⭐ 정상 응답인 경우에만 파일 삭제
            if (isValidResponse) {
                SetStatus(L"파일 삭제 중...");
                
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
                        #ifdef _DEBUG
                        std::wstring msg = L"파일 삭제 실패: " + p.wstring() + L" (에러코드: " + std::to_wstring(ec.value()) + L")\n";
                        OutputDebugStringW(msg.c_str());
                        #endif
                    }
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
        // allow moving by mouse position
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

    // load ini
    g_pMain->loadINI(L"./OCRChecker.ini");
    // if folder not exists, create it
    g_pMain->EnsureFolders();

    // set window size
    RECT rc = { 0, 0, 400, 200 };

    g_pMain->m_wndWidth = rc.right - rc.left;
    g_pMain->m_wndHeight = rc.bottom - rc.top;

    g_pMain->m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
    g_pMain->m_screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // calculate center position
    g_pMain->m_posX = (g_pMain->m_screenWidth - g_pMain->m_wndWidth) / 2;
    g_pMain->m_posY = (g_pMain->m_screenHeight - g_pMain->m_wndHeight) / 2;

    g_hWnd = ::CreateWindowW(wcex.lpszClassName, L"OCR Checker v1.0",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        g_pMain->m_posX, g_pMain->m_posY, g_pMain->m_wndWidth, g_pMain->m_wndHeight,
        NULL, NULL, g_hInst, NULL);

    g_pMain->m_dpi_response = 96.f / (float)GetDpiForSystem();

    ShowWindow(g_hWnd, true);
    UpdateWindow(g_hWnd);

    // start folder check timer
    SetTimer(g_hWnd, ID_TIMER_CHECK_FOLDER, 5000, NULL);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OCRCHECKER));

    MSG msg = { 0 };

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // clean up resources
    KillTimer(g_hWnd, ID_TIMER_CHECK_FOLDER);
    delete g_pMain;

    return (int)msg.wParam;
}
