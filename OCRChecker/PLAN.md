# OCRChecker 프로젝트 설계 문서

## 📋 프로젝트 개요

**프로젝트명**: OCRChecker
**목적**: 폴더를 타이머로 감시하여 플래그 파일(ocr.txt) 발견 시 이미지 파일을 읽어 OCR 서버로 전송하고 결과를 엑셀 파일에 기록하는 자동화 프로그램
**플랫폼**: Windows (WinAPI)
**언어**: C++ (C++17 이상)

---

## 🏗️ 시스템 아키텍처

### 전체 동작 흐름

```
[프로그램 시작]
    ↓
[INI 파일 로드]
    ↓
[폴더 생성 확인]
    ↓
[타이머 시작 (5초 간격)]
    ↓
    ↓────────────────────────────────┐
    ↓                                │
[CHECK_FOLDER 감시]                  │
    ↓                                │
[ocr.txt 있음?] ──No──→ [계속 감시]──┘
    ↓ Yes
[중복 처리 방지 플래그 설정]
[타이머 중지]
    ↓
[IMAGE_FOLDER에서 이미지 검색]
    ↓
[이미지 없음?] ──Yes──→ [ocr.txt 삭제] ─┐
    ↓ No                              │
[이미지 그룹화 (^0000 형식 처리)]      │
    ↓                                 │
[Base64 변환]                         │
    ↓                                 │
[JSON 생성]                           │
    ↓                                 │
[HTTP POST 전송]                      │
    ↓                                 │
[서버 응답 수신]                       │
    ↓                                 │
[엑셀 파일에 기록]                     │
    ↓                                 │
[이미지 파일 삭제]                     │
    ↓                                 │
[모든 이미지 처리 완료?] ──No──→ [반복]│
    ↓ Yes                             │
[ocr.txt 삭제]                        │
    ↓                                 │
[플래그 원복] ←───────────────────────┘
[타이머 재시작]
    ↓
[계속 감시...]
```

---

## 📁 프로젝트 파일 구조

```
OCRChecker/
├── main.cpp                 # 메인 로직 및 클래스 구현
├── main.h                   # ID 정의 (타이머, 버튼)
├── framework.h              # Windows 헤더 및 라이브러리 포함
├── Resource.h               # 리소스 ID 정의
├── targetver.h              # Windows SDK 버전 정의
├── OCRChecker.rc            # 리소스 스크립트
├── OCRChecker.vcxproj       # Visual Studio 프로젝트 파일
├── OCRChecker.ini           # 설정 파일 (실행 시 필요)
└── PLAN.md                  # 이 문서
```

---

## ⚙️ 설정 파일 (OCRChecker.ini)

**위치**: 실행 파일과 동일한 디렉토리

```ini
[CHECK_FOLDER]
PATH=C:\ocr

[IMAGE_FOLDER]
PATH=C:\img

[OCR_SERVER]
URL=127.0.0.1:18333
```

### 설정값 설명

- `CHECK_FOLDER/PATH`: ocr.txt 플래그 파일을 감시할 폴더
- `IMAGE_FOLDER/PATH`: 처리할 이미지 파일들이 위치한 폴더
- `OCR_SERVER/URL`: OCR 서버 주소 (host:port 형식)

---

## 🔧 핵심 클래스: CMain

### 주요 멤버 변수

```cpp
// 설정
AppConfig m_cfg;                        // INI에서 로드한 설정

// 상태 관리
std::atomic<bool> m_processing;         // 처리 중 플래그 (중복 방지)
std::atomic<bool> m_cancelRequested;    // 취소 요청 플래그

// 멀티스레드
std::thread m_worker;                   // 이미지 처리 워커 스레드
std::mutex m_mu;                        // 뮤텍스

// UI 컨트롤
HWND m_btnStart;                        // 강제시작 버튼
HWND m_btnCancel;                       // 강제취소 버튼
HWND m_status;                          // 상태 텍스트
```

### 주요 메서드

#### 1. 초기화 및 설정

```cpp
bool loadINI(const std::wstring& filePath);
void EnsureFolders();
```

#### 2. 타이머 제어

```cpp
void StartTimer();                      // 5초 간격 타이머 시작
void StopTimer();                       // 타이머 중지
```

#### 3. 폴더 감시 및 처리

```cpp
void checkFolder();                     // ocr.txt 확인 및 처리 시작
void ProcessImagesLoop();               // 이미지 처리 메인 루프 (워커 스레드)
```

#### 4. 이미지 처리

```cpp
static std::wstring GetNameRoot(const std::wstring& filename);
static std::string ReadFileToBase64(const std::wstring& path);
static std::string Base64Encode(const unsigned char* data, size_t len);
```

#### 5. HTTP 통신

```cpp
static bool HttpPostJson(
    const std::wstring& hostAndPort,
    const std::wstring& path,
    const std::string& json,
    std::string& outResponse,
    DWORD timeoutMs
);
```

#### 6. 파일 관리

```cpp
void DeleteFlagFile();                  // ocr.txt 삭제
void AppendLinesToExcelCsvCompat(const std::vector<std::string>& lines);
```

#### 7. 사용자 제어

```cpp
void ForceStart();                      // 강제 시작
void ForceCancel();                     // 강제 취소
```

---

## 📝 상세 동작 시나리오

### 시나리오 1: 정상 처리 흐름

1. **타이머 이벤트 발생** (5초마다)

   ```cpp
   WM_TIMER → checkFolder()
   ```
2. **ocr.txt 확인**

   ```cpp
   if (!fs::exists(flag)) return;  // 없으면 리턴
   ```
3. **중복 처리 방지**

   ```cpp
   if (m_processing.load()) return;  // 이미 처리 중이면 리턴
   m_processing.store(true);         // 플래그 설정
   ```
4. **타이머 중지 및 워커 스레드 시작**

   ```cpp
   StopTimer();
   m_worker = std::thread([this]() { this->ProcessImagesLoop(); });
   ```
5. **이미지 검색 및 정렬**

   ```cpp
   for (auto& entry : fs::directory_iterator(imgDir)) {
       if (IsImageFileExt(p.extension().wstring())) {
           all.push_back(p);
       }
   }
   std::sort(all.begin(), all.end());
   ```
6. **이미지가 없는 경우**

   ```cpp
   if (all.empty()) {
       DeleteFlagFile();  // ocr.txt 삭제
       break;
   }
   ```
7. **파일명 그룹화** (^0001, ^0002... 형식)

   ```cpp
   // "홍길동^0001.png", "홍길동^0002.png" → "홍길동"으로 그룹화
   std::wstring root = GetNameRoot(name0);
   for (auto& p : all) {
       if (GetNameRoot(p.filename().wstring()) == root) {
           files.push_back(p);
       }
   }
   ```
8. **JSON 생성**

   ```json
   [
       {"filename": "홍길동^0001.png", "data": "base64string..."},
       {"filename": "홍길동^0002.png", "data": "base64string..."}
   ]
   ```
9. **HTTP POST 전송**

   ```cpp
   HttpPostJson(m_cfg.ocrServer, L"/ocr", json, resp, 10 * 60 * 1000);
   ```
10. **응답 저장**

    ```cpp
    AppendLinesToExcelCsvCompat({ resp });
    ```
11. **이미지 파일 삭제**

    ```cpp
    for (auto& p : files) {
        fs::remove(p, ec);
    }
    ```
12. **반복 또는 완료**

    - 이미지가 남아있으면 다음 그룹 처리
    - 모든 이미지 처리 완료 시 ocr.txt 삭제
13. **플래그 원복 및 타이머 재시작**

    ```cpp
    m_processing.store(false);
    StartTimer();
    ```

### 시나리오 2: 이미지 없음

```
ocr.txt 발견 → IMAGE_FOLDER 확인 → 이미지 없음 
→ ocr.txt 삭제 → 플래그 원복 → 타이머 재시작
```

### 시나리오 3: 강제 취소

```
사용자가 [강제취소] 버튼 클릭
→ m_cancelRequested = true
→ 서버에 취소 요청 전송 (/cancel)
→ ocr.txt 삭제
→ 워커 스레드 종료 대기
→ 플래그 원복
→ 타이머 재시작
```

---

## 🖥️ GUI 구성

### 윈도우 크기

- 가로: 400px
- 세로: 200px
- 위치: 화면 중앙
- 스타일: 크기 조절 불가, 최대화 불가

### UI 요소

```
┌─────────────────────────────────────┐
│  OCR Checker                        │
├─────────────────────────────────────┤
│                                     │
│  [강제시작]  [강제취소]              │
│                                     │
│  상태: 감시중                        │
│                                     │
└─────────────────────────────────────┘
```

#### 버튼

1. **강제시작** (ID_BTN_FORCE_START: 40001)

   - 위치: (20, 20), 크기: (120, 30)
   - 기능: ocr.txt 없이 강제로 처리 시작
2. **강제취소** (ID_BTN_FORCE_CANCEL: 40002)

   - 위치: (160, 20), 크기: (120, 30)
   - 기능: 처리 중단 및 초기화

#### 상태 텍스트 (STATIC)

- 위치: (20, 60), 크기: (400, 20)
- 표시 내용:
  - "감시중" (대기 상태)
  - "처리중" (이미지 처리 중)
  - "서버에 전송 중..." (HTTP 전송 중)
  - "서버 응답 받음, 파일 삭제 중..." (처리 완료)
  - "처리할 이미지가 없습니다" (이미지 없음)
  - "서버 통신 실패" (오류)
  - "취소됨" (사용자 취소)

---

## 🔒 동시성 제어

### 1. 중복 처리 방지

```cpp
std::atomic<bool> m_processing;

void checkFolder() {
    if (m_processing.load()) return;  // 이미 처리 중이면 리턴
    m_processing.store(true);
    // ... 처리 ...
    m_processing.store(false);
}
```

### 2. 취소 요청 처리

```cpp
std::atomic<bool> m_cancelRequested;

while (!m_cancelRequested.load()) {
    // 처리 로직
    if (m_cancelRequested.load()) break;
}
```

### 3. 스레드 안전한 종료

```cpp
~CMain() {
    RequestCancel();
    if (m_worker.joinable()) m_worker.join();
}
```

---

## 📡 HTTP 통신 프로토콜

### OCR 요청

**Endpoint**: `POST /ocr`
**Content-Type**: `application/json; charset=utf-8`
**Timeout**: 10분 (600,000ms)

**Request Body**:

```json
[
    {
        "filename": "홍길동^0001.png",
        "data": "iVBORw0KGgoAAAANSUhEUgAA..."
    },
    {
        "filename": "홍길동^0002.png",
        "data": "iVBORw0KGgoAAAANSUhEUgAA..."
    }
]
```

**Response**:

```
서버가 반환하는 문자열 (엑셀에 기록됨)
```

### 취소 요청

**Endpoint**: `POST /cancel`
**Content-Type**: `application/json; charset=utf-8`
**Timeout**: 10초 (10,000ms)

**Request Body**:

```json
{
    "cancel": true
}
```

---

## 📊 파일 처리 규칙

### 이미지 파일 형식

- 확장자: `.png`, `.jpg`, `.jpeg` (대소문자 구분 없음)

### 파일명 그룹화 규칙

#### 예시 1: 단일 파일

```
김철수.png
```

→ 단독 처리

#### 예시 2: 연속 파일

```
이영희^0001.png
이영희^0002.png
이영희^0003.png
```

→ 한 번에 배열로 묶어서 전송

#### 예시 3: 혼합

```
김철수.png
이영희^0001.png
이영희^0002.png
박민수^0001.png
```

처리 순서:

1. `김철수.png` (단독)
2. `이영희^0001.png`, `이영희^0002.png` (그룹)
3. `박민수^0001.png` (단독)

### 그룹화 로직

```cpp
static std::wstring GetNameRoot(const std::wstring& filename) {
    // "홍길동^0001.png" → "홍길동"
    size_t pos = filename.find(L'^');
    if (pos != std::wstring::npos)
        return filename.substr(0, pos);
  
    // "김철수.png" → "김철수"
    return std::filesystem::path(filename).stem().wstring();
}
```

---

## 💾 출력 파일

### 엑셀 파일

**경로**: `CHECK_FOLDER/활동데이터_견적서템플릿.xlsx`
**예시**: `C:\ocr\활동데이터_견적서템플릿.xlsx`

**저장 방식**:

- 파일 끝에 추가 (append mode)
- 서버 응답 문자열을 줄바꿈으로 구분하여 기록
- 인코딩: UTF-8 (BOM 없음)

```
서버응답1
서버응답2
서버응답3
...
```

---

## 🐛 디버그 모드

`#ifdef _DEBUG` 블록에서 상세한 로그 출력

### 디버그 로그 항목

1. **HTTP 통신**

   - WinHttpOpen 실패
   - WinHttpConnect 실패
   - WinHttpOpenRequest 실패
   - WinHttpSendRequest 실패
   - WinHttpReceiveResponse 실패
2. **이미지 처리**

   - 찾은 이미지 파일 수 및 목록
   - 파일 읽기 실패
   - 전송할 JSON 크기
3. **서버 통신**

   - 서버 통신 결과 (성공/실패)
   - 서버 응답 크기 및 내용
4. **파일 삭제**

   - 삭제할 파일 수
   - 파일 삭제 성공/실패
   - 에러 코드
5. **최종 상태**

   - anyProcessed, errorOccurred, cancelRequested
   - 플래그 파일 삭제 시도
   - 처리 상태 초기화 및 타이머 재시작

### 디버그 출력 확인

Visual Studio의 Output 창에서 확인:

```
Debug → Windows → Output
```

---

## 🔑 핵심 함수 코드 스니펫

### 1. INI 파일 로드

```cpp
bool loadINI(const std::wstring& filePath) {
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

    if (!ok) {
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
```

### 2. Base64 인코딩

```cpp
static std::string Base64Encode(const unsigned char* data, size_t len) {
    static const char b64_table[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  
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
  
    // 패딩 처리
    if (i < len) {
        unsigned triple = data[i] << 16;
        if (i + 1 < len) triple |= data[i + 1] << 8;
        out.push_back(b64_table[(triple >> 18) & 0x3F]);
        out.push_back(b64_table[(triple >> 12) & 0x3F]);
        if (i + 1 < len) {
            out.push_back(b64_table[(triple >> 6) & 0x3F]);
            out.push_back('=');
        } else {
            out.push_back('=');
            out.push_back('=');
        }
    }
  
    return out;
}
```

### 3. JSON 이스케이프

```cpp
static std::string EscapeJson(const std::string& s) {
    std::string out;
    out.reserve(s.size());
  
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
            } else {
                out += c;
            }
        }
    }
  
    return out;
}
```

---

## 🛠️ 빌드 설정

### 필수 라이브러리

```cpp
#pragma comment(lib, "winhttp.lib")
```

### 필수 헤더

```cpp
#include <windows.h>        // Windows API
#include <winhttp.h>        // HTTP 통신
#include <filesystem>       // 파일 시스템
#include <atomic>           // 원자적 연산
#include <thread>           // 멀티스레드
#include <mutex>            // 뮤텍스
```

### C++ 표준

- C++17 이상 필요 (std::filesystem 사용)

### Visual Studio 설정

1. **Character Set**: Unicode
2. **C++ Language Standard**: C++17 이상
3. **Additional Dependencies**: winhttp.lib

---

## 📋 체크리스트: 코드 재생성 시 확인 사항

### ✅ 필수 구현 요소

- [ ] INI 파일 로드 (`CHECK_FOLDER`, `IMAGE_FOLDER`, `OCR_SERVER`)
- [ ] 5초 간격 타이머 (`SetTimer`, `ID_TIMER_CHECK_FOLDER`)
- [ ] ocr.txt 감시 및 중복 처리 방지 (`m_processing`)
- [ ] 이미지 파일 검색 (`.png`, `.jpg`, `.jpeg`)
- [ ] 파일명 그룹화 (`^0001` 형식 처리)
- [ ] Base64 인코딩
- [ ] JSON 배열 생성 및 이스케이프
- [ ] HTTP POST 전송 (WinHTTP 사용)
- [ ] 10분 타임아웃 설정
- [ ] 서버 응답 엑셀 파일 저장 (append mode)
- [ ] 처리된 이미지 파일 삭제
- [ ] 모든 이미지 처리 후 ocr.txt 삭제
- [ ] 이미지 없을 때 ocr.txt 삭제
- [ ] 플래그 원복 및 타이머 재시작
- [ ] 강제시작/강제취소 버튼
- [ ] 상태 텍스트 표시
- [ ] 워커 스레드 안전한 종료
- [ ] 취소 요청 처리 (`m_cancelRequested`)

### ✅ 주요 에러 처리

- [ ] INI 파일 없을 때 에러 메시지
- [ ] 폴더 없을 때 자동 생성
- [ ] HTTP 연결 실패 처리
- [ ] 이미지 읽기 실패 처리
- [ ] 파일 삭제 실패 로그 (디버그)

### ✅ UI/UX

- [ ] 윈도우 크기 조절 불가
- [ ] 윈도우 중앙 배치
- [ ] 윈도우 드래그 이동 가능 (`WM_NCHITTEST`)
- [ ] 종료 확인 메시지박스
- [ ] 강제취소 버튼 (처리 중이 아니면 무시)

---

## 🚀 프로젝트 재생성 가이드

### 1단계: 프로젝트 생성

```
Visual Studio → New Project → Windows Desktop Application
Project Name: OCRChecker
```

### 2단계: 파일 구성

1. `framework.h` - Windows 헤더 및 라이브러리
2. `main.h` - ID 정의
3. `Resource.h` - 리소스 ID
4. `main.cpp` - 메인 로직 (CMain 클래스)

### 3단계: 핵심 클래스 구현

```cpp
class CMain {
    // 상태 관리
    std::atomic<bool> m_processing;
    std::atomic<bool> m_cancelRequested;
    std::thread m_worker;
  
    // 설정
    AppConfig m_cfg;
  
    // UI
    HWND m_btnStart, m_btnCancel, m_status;
  
    // 메서드
    bool loadINI(const std::wstring& filePath);
    void checkFolder();
    void ProcessImagesLoop();
    static bool HttpPostJson(...);
    static std::string Base64Encode(...);
};
```

### 4단계: 메시지 루프 구현

```cpp
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        if (wParam == ID_TIMER_CHECK_FOLDER)
            g_pMain->checkFolder();
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BTN_FORCE_START)
            g_pMain->ForceStart();
        else if (LOWORD(wParam) == ID_BTN_FORCE_CANCEL)
            g_pMain->ForceCancel();
        break;
    // ...
    }
}
```

### 5단계: INI 파일 생성

실행 파일 경로에 `OCRChecker.ini` 생성

### 6단계: 테스트

1. `C:\ocr` 폴더 생성
2. `C:\img` 폴더에 이미지 추가
3. `C:\ocr\ocr.txt` 생성
4. 프로그램 실행 확인

---

## 📚 참고사항

### WinHTTP vs WinINet

- 이 프로젝트는 **WinHTTP** 사용 (서버 애플리케이션에 적합)
- WinINet은 브라우저 기반 애플리케이션에 적합

### 멀티바이트 vs 유니코드

- 프로젝트는 **유니코드** 사용 (`std::wstring`, `L"..."`)
- JSON은 UTF-8로 변환 (`WideToUtf8`)

### 파일 시스템

- C++17 `std::filesystem` 사용
- `std::error_code`로 예외 없는 에러 처리

---

## 🔗 관련 문서

- [proj.md](proj.md) - 원본 프로젝트 설명 (한글 인코딩)
- [main.cpp](main.cpp) - 메인 소스 코드
- [main.h](main.h) - ID 정의
- [framework.h](framework.h) - 헤더 파일

---

## 📌 버전 정보

- **작성일**: 2025-10-18
- **Visual Studio**: 2022 (VC143)
- **Windows SDK**: 10.0
- **C++ Standard**: C++17

---

## ⚠️ 주의사항

1. **타이머와 스레드 동기화**

   - `m_processing` 플래그로 중복 처리 방지 필수
2. **파일 삭제 시점**

   - 이미지 없을 때: 즉시 ocr.txt 삭제
   - 처리 완료 후: ocr.txt 삭제
3. **그룹화 처리**

   - 첫 번째 파일의 root로 그룹 결정
   - 정렬 후 순차 처리
4. **HTTP 타임아웃**

   - OCR: 10분 (600,000ms)
   - Cancel: 10초 (10,000ms)
5. **스레드 안전**

   - `std::atomic` 사용
   - 종료 시 `join()` 필수
6. **인코딩**

   - 내부: UTF-16 (wstring)
   - JSON: UTF-8 (string)
   - 파일명: UTF-8로 변환 후 전송

---

**이 문서를 기반으로 프로젝트 전체를 재생성할 수 있습니다.**
