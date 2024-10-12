#include "pch.h"
#include "DlgClass.h"
#include "resource.h"
#include "AccelerationCoefficientLib.h"

INT_PTR DlgClass::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        OnPaint();
    }
    return (INT_PTR)TRUE;
    case WM_COMMAND:
    {
        OnCommand(LOWORD(wParam));
    }
    return (INT_PTR)TRUE;
    case WM_INITDIALOG:
        OnInitialDialog();
        return (INT_PTR)TRUE;
    case WM_DESTROY:
        OnDestroy();
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_USER +1:
        double* pResult = reinterpret_cast<double*>(lParam);
        double result = *pResult;
        std::wstring lstr = L"작업이 완료되었습니다. 가속계수: " + std::to_wstring(result);

        // 버튼을 다시 활성화
        SetDlgItemText(m_hWnd, IDC_TEXT_HEADER, lstr.c_str());
        EnableWindow(GetDlgItem(m_hWnd, IDOK), TRUE);
        EnableWindow(GetDlgItem(m_hWnd, IDCANCEL), TRUE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_TEMPERATURE), TRUE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_HUMIDITY), TRUE);
        EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SELECT_FILE), TRUE);
        MessageBox(GetActiveWindow(), lstr.c_str(), L"가속계수산출 결과", MB_OK);
        
        break;
    }
    return (INT_PTR)FALSE;
}

void DlgClass::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hWnd, &ps);
    // TODO: Add any drawing code that uses hdc here...
    //Rectangle(hdc, 10, 10, 120, 120);

    EndPaint(m_hWnd, &ps);
}

void DlgClass::OnDestroy()
{
    PostQuitMessage(0);
}

void DlgClass::OnCommand(int id)
{
    if (id == IDCANCEL)
    {
        //EndDialog(m_hWnd, id);
        // EditControl을 비운다
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_TEMPERATURE), _T(""));
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_HUMIDITY), _T(""));
        SetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_FILE_PATH), _T(""));
    }
    else if (id == IDOK) {
        //MessageBox(m_hWnd, _T("id"), _T("ok"), MB_OK);
        // EditControl의 두 값을 가져와서
        // 실수형으로 바꾸고
        // 라이브러리를 호출한다
        TCHAR szEdit1[100], szEdit2[100];
        wchar_t* dump1 = NULL;
        wchar_t* dump2 = NULL;
        GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_TEMPERATURE), szEdit1, 100);
        GetWindowText(GetDlgItem(m_hWnd, IDC_EDIT_HUMIDITY), szEdit2, 100);

        double dbValue1 = wcstod(szEdit1, &dump1);
        double dbValue2 = wcstod(szEdit2, &dump2);

        if (*dump1 != L'\0' || *dump2 != L'\0') {
            MessageBox(GetActiveWindow(), L"숫자를 입력해주세요", L"결과", MB_OK);
        }
        else {
            EnableWindow(GetDlgItem(m_hWnd, IDOK), FALSE);
            EnableWindow(GetDlgItem(m_hWnd, IDCANCEL), FALSE);
            EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_TEMPERATURE), FALSE);
            EnableWindow(GetDlgItem(m_hWnd, IDC_EDIT_HUMIDITY), FALSE);
            EnableWindow(GetDlgItem(m_hWnd, IDC_BUTTON_SELECT_FILE), FALSE);

            std::wstring lstr = L"작업이 진행 중이에요. 잠시만 기다려주세요. [" + std::to_wstring(dbValue1) + L"/" + std::to_wstring(dbValue2) + L"]";
            SetDlgItemText(m_hWnd, IDC_TEXT_HEADER, lstr.c_str());

            // 라이브러리 동작 (선택 파일 경로 추가 필요)
            double result = runAcceleration(m_strFileFullPath.c_str(), dbValue1, dbValue2);

            double* pResult = new double(result);
            PostMessage(m_hWnd, WM_USER + 1, 0, reinterpret_cast<LPARAM>(pResult));

            //MessageBox(GetActiveWindow(), lstr.c_str(), L"결과", MB_OK);
        }
    }
    else if (id == IDC_BUTTON_SELECT_FILE) {
        operateButtonSelectFile();
    }
}

void DlgClass::OnInitialDialog()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    RECT rect;
    GetWindowRect(m_hWnd, &rect);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    SetWindowPos(m_hWnd, HWND_TOP, posX, posY, windowWidth, windowHeight, SWP_NOZORDER | SWP_NOSIZE);
    SetDlgItemText(m_hWnd, IDC_TEXT_HEADER, L"가속계수 입력 예시값 : 333.15 / 90.0");

    SetDlgItemText(m_hWnd, IDC_EDIT_FILE_PATH, L"");
}

// 생성자
DlgClass::DlgClass()
    : m_hWnd(NULL)
{
    m_strFileFullPath.clear();
}

// 소멸자
DlgClass::~DlgClass()
{
}

// 복사 생성자
DlgClass::DlgClass(const DlgClass& other)
    : m_hWnd(other.m_hWnd), m_strFileFullPath(other.m_strFileFullPath)
{
}

// 복사 대입 연산자
DlgClass& DlgClass::operator=(const DlgClass& other)
{
    if (this != &other) {
        m_hWnd = other.m_hWnd;
        m_strFileFullPath = other.m_strFileFullPath;
    }
    return *this;
}

// 이동 생성자
DlgClass::DlgClass(DlgClass&& other) noexcept
    : m_hWnd(other.m_hWnd), m_strFileFullPath(std::move(other.m_strFileFullPath))
{
    other.m_hWnd = NULL;
}

// 이동 대입 연산자
DlgClass& DlgClass::operator=(DlgClass&& other) noexcept
{
    if (this != &other) {
        m_hWnd = other.m_hWnd;
        m_strFileFullPath = std::move(other.m_strFileFullPath);
        other.m_hWnd = NULL;
    }
    return *this;
}

void DlgClass::operateButtonSelectFile()
{
    char szFile[_MAX_PATH] = { 0 };

    OPENFILENAMEA ofn;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = "Excel Files(.xlsx)\0*.xlsx\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        SetDlgItemTextA(m_hWnd, IDC_EDIT_FILE_PATH, szFile);
        m_strFileFullPath = std::string(szFile);                // 선택된 파일경로를 멤버변수에 담는다
    }
}
