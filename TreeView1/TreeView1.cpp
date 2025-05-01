#include "framework.h"
#include "TreeView1.h"
#include <windowsx.h>
#include <CommCtrl.h>
#include <Shlwapi.h>
#include <gdiplus.h>
using namespace Gdiplus;

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "gdiplus.lib")

HINSTANCE g_hInst;
HWND g_hWnd;

HWND g_hTreeView;
HIMAGELIST g_hBtnImg;
HIMAGELIST g_hStateCircleImg;

ULONG_PTR g_gdiplusToken = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_NOTIFY:
    {
        LPNMHDR pnm = (LPNMHDR)lParam;
        if (pnm->idFrom == IDC_MY_TREE)
        {
            if (pnm->code == NM_DBLCLK)
            {
                // 더블클릭 알림을 무시하면, 트리뷰가 기본 동작(토글)을 안 합니다.
                return TRUE;
            }
            // 1) 체크박스(화살표) 자리 클릭 감지
            else if (pnm->code == NM_CLICK)
            {
                // 화면 좌표 → 트리뷰 좌표
                DWORD pos = GetMessagePos();
                POINT pt = { GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };
                ScreenToClient(g_hTreeView, &pt);

                // 히트 테스트
                TVHITTESTINFO hti = { pt };
                HTREEITEM hItem = TreeView_HitTest(g_hTreeView, &hti);

                // state‐icon 또는 텍스트 클릭 시만 처리
                const UINT mask = TVHT_ONITEMSTATEICON | TVHT_ONITEMLABEL;
                if (hItem && (hti.flags & mask))
                {
                    // 1) 현재 state‐image 인덱스 얻기 (1 또는 2)
                    UINT state = TreeView_GetItemState(g_hTreeView, hItem, TVIS_STATEIMAGEMASK);
                    int idx = state >> 12;  // high nibble

                    // 2) 인덱스가 1,2가 아니면(=0) 아무 동작 안 함
                    if (idx == 0) {
                        return 0;
                    }

                    // 2) 토글
                    int next = 1;
                    if (idx == 2) {
                        next = 3;
                    }
                    else if(idx == 3) {
                        next = 2;
                    }
                    else {
                        return 0;
                    }

                    // 3) 아이템에 새 state 설정
                    TVITEMW ti = {};
                    ti.hItem = hItem;
                    ti.mask = TVIF_STATE;
                    ti.stateMask = TVIS_STATEIMAGEMASK;
                    ti.state = INDEXTOSTATEIMAGEMASK(next);
                    TreeView_SetItem(g_hTreeView, &ti);

                    // 4) (옵션) 토글에 맞추어 펼치기/접기
                    TreeView_Expand(
                        g_hTreeView,
                        hItem,
                        next == 3 ? TVE_EXPAND : TVE_COLLAPSE
                    );
                }
            }
            else if (pnm->code == NM_CUSTOMDRAW)
            {
                LPNMTVCUSTOMDRAW pTvcd = (LPNMTVCUSTOMDRAW)pnm;
                DWORD stage = pTvcd->nmcd.dwDrawStage;
                if (stage == CDDS_PREPAINT)
                {
                    return CDRF_NOTIFYITEMDRAW;
                }
                else if (stage == CDDS_ITEMPREPAINT)
                {
                    return CDRF_NOTIFYPOSTPAINT;
                }
                else if (stage == CDDS_ITEMPOSTPAINT)
                {

                    return CDRF_SKIPDEFAULT;
                }
                return CDRF_DODEFAULT;
            }
            else if (pnm->code == TVN_KEYDOWN)
            {
                // 임시 테스트용 키다운 (회색 -> 빨강 -> 주황 -> 초록)
                LPNMTVKEYDOWN pk = (LPNMTVKEYDOWN)pnm;
                if (pk->wVKey == VK_RETURN)
                {
                    HTREEITEM hSel = TreeView_GetSelection(g_hTreeView);
                    if (hSel)
                    {
                        TVITEMW ti = { 0 };
                        ti.hItem = hSel;
                        ti.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                        TreeView_GetItem(g_hTreeView, &ti);
                        int next = (ti.iImage +1) % 4;              // 0 ~ 3 으로 색상을 지정한다
                        ti.iImage = next;
                        ti.iSelectedImage = next;
                        TreeView_SetItem(g_hTreeView, &ti);
                    }
                }
            }
        }

        

        // (기존에 쓰고 계신 다른 NM_NOTIFY 핸들링 아래에 그대로 두시면 됩니다.)
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    g_hInst = hInstance;                    // 인스턴스 핸들을 전역 변수에 저장합니다.

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    // Register Class
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = g_hInst;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(250, 250, 250));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Test1";
    wcex.hIconSm = NULL;
    RegisterClassExW(&wcex);

    // 창의 크기를 지정
    RECT rc = { 0, 0, 640, 480 };
    ::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);              // 여백보정

    g_hWnd = ::CreateWindowW(L"Test1", L"TreeView Custom",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL, NULL, g_hInst, NULL);

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    // 단축키 테이블
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TREEVIEW1));

    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_TREEVIEW_CLASSES };
    InitCommonControlsEx(&icex);

    g_hTreeView = CreateWindowEx(
        NULL,
        //WS_EX_CLIENTEDGE,
        L"SysTreeView32",
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | TVS_HASLINES | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS /*| TVS_CHECKBOXES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_BORDER*/,
        30, 30, 300, 400,
        g_hWnd, (HMENU)IDC_MY_TREE,
        g_hInst, NULL
    );

    TreeView_SetExtendedStyle(
        g_hTreeView,
        TVS_EX_DOUBLEBUFFER,
        TVS_EX_DOUBLEBUFFER
    );

    const int bmp_size = 24;
    // 1) State용 ImageList 생성 (예: 오른쪽 화살표→인덱스1, 아래 화살표→인덱스2)
    g_hBtnImg = ImageList_Create(bmp_size, bmp_size, ILC_COLOR32 | ILC_MASK, 3, 1);

    HBITMAP hBmpBlank = CreateCompatibleBitmap(GetDC(nullptr), bmp_size, bmp_size);
    HDC mdc = CreateCompatibleDC(nullptr);
    SelectObject(mdc, hBmpBlank);
    HBRUSH br = CreateSolidBrush(RGB(255, 255, 255));               // 배경색과 같게
    RECT blankRECT = { 0, 0, bmp_size, bmp_size };
    FillRect(mdc, &blankRECT, br);
    DeleteObject(br); 
    DeleteDC(mdc);

    wchar_t exePath[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    PathRemoveFileSpecW(exePath);
    wchar_t bmpRightPath[MAX_PATH] = { 0 };
    PathCombineW(bmpRightPath, exePath, L"res\\arrow_right_32dp_B7B7B7.bmp");
    wchar_t bmpDownPath[MAX_PATH] = { 0 };
    PathCombineW(bmpDownPath, exePath, L"res\\arrow_drop_down_32dp_B7B7B7.bmp");

    HBITMAP hBmpRight = (HBITMAP)LoadImageW(NULL, bmpRightPath, IMAGE_BITMAP, bmp_size, bmp_size, LR_LOADFROMFILE);
    HBITMAP hBmpDown = (HBITMAP)LoadImageW(NULL, bmpDownPath, IMAGE_BITMAP, bmp_size, bmp_size, LR_LOADFROMFILE);

    ImageList_AddMasked(g_hBtnImg, hBmpBlank, RGB(255, 0, 255));        // 인덱스 0
    ImageList_AddMasked(g_hBtnImg, hBmpBlank, RGB(255, 0, 255));        // 인덱스 1
    ImageList_AddMasked(g_hBtnImg, hBmpRight, RGB(255, 0, 255));        // 인덱스 2
    ImageList_AddMasked(g_hBtnImg, hBmpDown, RGB(255, 0, 255));         // 인덱스 3

    // 2) 트리뷰에 State ImageList 설정
    TreeView_SetImageList(g_hTreeView, g_hBtnImg, TVSIL_STATE);

    // CIRCLE : gray(0), red(1), yellow(2), green(3)
    g_hStateCircleImg = ImageList_Create(bmp_size, bmp_size, ILC_COLOR32 | ILC_MASK, 4, 1);
    HBITMAP bmpGray = CreateGdiPlusCircleBitmap(bmp_size, RGB(129, 129, 129));
    HBITMAP bmpRed = CreateGdiPlusCircleBitmap(bmp_size, RGB(255, 0, 0));
    HBITMAP bmpYellow = CreateGdiPlusCircleBitmap(bmp_size, RGB(192, 192, 101));
    HBITMAP bmpGreen = CreateGdiPlusCircleBitmap(bmp_size, RGB(0, 172, 0));

    ImageList_AddMasked(g_hStateCircleImg, bmpGray, RGB(255, 0, 255));
    ImageList_AddMasked(g_hStateCircleImg, bmpRed, RGB(255, 0, 255));
    ImageList_AddMasked(g_hStateCircleImg, bmpYellow, RGB(255, 0, 255));
    ImageList_AddMasked(g_hStateCircleImg, bmpGreen, RGB(255, 0, 255));
    TreeView_SetImageList(g_hTreeView, g_hStateCircleImg, TVSIL_NORMAL);

    TVINSERTSTRUCT tvis = {};
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvis.item.stateMask = TVIS_STATEIMAGEMASK;
    tvis.item.state = INDEXTOSTATEIMAGEMASK(2);     // 오른쪽 화살표
    tvis.item.iImage = 0; 
    tvis.item.iSelectedImage = 0;

    // [1] 최상위 루트
    tvis.hParent = TVI_ROOT;
    tvis.item.pszText = (LPWSTR)L"최상위1";
    HTREEITEM hRoot = TreeView_InsertItem(g_hTreeView, &tvis);

    // [2] 루트의 두 자식
    const wchar_t* childNames[2] = { L"자식 노드 A", L"자식 노드 B" };
    HTREEITEM children[2];

    tvis.hParent = hRoot;
    tvis.item.pszText = (LPWSTR)childNames[0];
    tvis.item.state = INDEXTOSTATEIMAGEMASK(1);     // 화살표 없음
    tvis.item.iImage = 0;
    tvis.item.iSelectedImage = 0;
    children[0] = TreeView_InsertItem(g_hTreeView, &tvis);

    tvis.hParent = hRoot;
    tvis.item.pszText = (LPWSTR)childNames[1];
    tvis.item.state = INDEXTOSTATEIMAGEMASK(2);     // 오른쪽 화살표
    children[1] = TreeView_InsertItem(g_hTreeView, &tvis);

    // 3) 각 자식 아래에 두 개씩 손자 노드
    const wchar_t* grandNames[] = { L"손자 노드 1", L"손자 노드 2" };

        for (int j = 0; j < 2; ++j) {
            tvis.hParent = children[1];
            tvis.item.state = INDEXTOSTATEIMAGEMASK(1);     // 화살표 없음
            tvis.item.pszText = (LPWSTR)grandNames[j];
            TreeView_InsertItem(g_hTreeView, &tvis);
        }
    

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        // DirectX는 항상 렌더링 될 수 있게 PeekMessage를 사용해야한다
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 메시지 처리
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // 엔진 렌더링 수행
            
        }
    }

    GdiplusShutdown(g_gdiplusToken);

    return (int)msg.wParam;
}


HBITMAP createCircleBitmap(int circle_size, COLORREF fillColor, COLORREF maskColor)
{
    // 1) 32비트 ARGB DIB 섹션(TOP-DOWN) 생성
    BITMAPINFO bi = { 0 };
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = circle_size;
    bi.bmiHeader.biHeight = -circle_size;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* pvBits = nullptr;
    HBITMAP hBmp = CreateDIBSection( NULL, &bi, DIB_RGB_COLORS,&pvBits, NULL, 0);
    if (!hBmp) return NULL;

    // 2) 메모리 DC에 비트맵 선택
    HDC hdcMem = CreateCompatibleDC(NULL);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);

    // 3) 전체 배경을 maskColor 로 채워서 투명 마스크 준비
    HBRUSH hBrMask = CreateSolidBrush(maskColor);
    RECT   rc = { 0,0,circle_size,circle_size };
    FillRect(hdcMem, &rc, hBrMask);
    DeleteObject(hBrMask);

    // 4) 펜 없이(fill 전용) 브러시로 원 그리기
    HBRUSH hBrFill = CreateSolidBrush(fillColor);
    HPEN   hOldPen = (HPEN)SelectObject(hdcMem, GetStockObject(NULL_PEN));
    SelectObject(hdcMem, hBrFill);
    Ellipse(hdcMem, 0, 0, circle_size, circle_size);
    DeleteObject(hBrFill);

    // 5) 클린업
    SelectObject(hdcMem, hOldPen);
    SelectObject(hdcMem, hOld);
    DeleteDC(hdcMem);

    return hBmp;
}

// GDI+로 원 비트맵 생성
// circle_size: 비트맵 크기(px)
// fillColor : 원을 채울 RGB 색상
HBITMAP CreateGdiPlusCircleBitmap(int circle_size, COLORREF fillColor)
{
    // 1) GDI+ Bitmap 객체 생성 (ARGB)
    Bitmap bmp(circle_size, circle_size, PixelFormat32bppARGB);

    // 2) 고품질 안티앨리어싱 설정
    HDC screenDC = GetDC(NULL);
    float dpiX = (float)GetDeviceCaps(screenDC, LOGPIXELSX);
    float dpiY = (float)GetDeviceCaps(screenDC, LOGPIXELSY);
    ReleaseDC(NULL, screenDC);
    bmp.SetResolution(dpiX, dpiY);

    Graphics g(&bmp);
    g.SetSmoothingMode(SmoothingModeHighQuality);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);

    // 2) 배경을 완전 투명(ARGB=0)으로 지우기
    Color bk(0, GetRValue(fillColor), GetGValue(fillColor), GetBValue(fillColor));
    Color circle(255, GetRValue(fillColor), GetGValue(fillColor), GetBValue(fillColor));
    g.Clear(bk);

    // 3) 솔리드 브러시로 원 그리기 (풀 불투명)
    SolidBrush brFill(circle);
    int diameter = circle_size * 75 / 100;            // 전체 크기의 75%
    int inset = (circle_size - diameter) / 2;     // 남은 공간을 반으로 나눠서
    g.FillEllipse(&brFill, inset, inset, diameter, diameter);

    // 4) GDI+ Bitmap → HBITMAP (알파 채널 유지)
    HBITMAP hBmp = nullptr;
    bmp.GetHBITMAP(bk, &hBmp);
    return hBmp;
}