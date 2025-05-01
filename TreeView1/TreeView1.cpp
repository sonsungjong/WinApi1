#include "framework.h"
#include "TreeView1.h"
#include <windowsx.h>
#include <CommCtrl.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "comctl32.lib")

HINSTANCE g_hInst;
HWND g_hWnd;

HWND g_hTreeView;
HIMAGELIST g_hBtnImg;

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
                    int next = (idx == 1 ? 2 : 1);

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
                        next == 2 ? TVE_EXPAND : TVE_COLLAPSE
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
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
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

    g_hWnd = ::CreateWindowW(L"Test1", L"Blank Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, g_hInst, nullptr);

    ShowWindow(g_hWnd, true);
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

    // 1) State용 ImageList 생성 (예: 오른쪽 화살표→인덱스1, 아래 화살표→인덱스2)
    g_hBtnImg = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 0);

    HBITMAP hBmpBlank = CreateBitmap(16, 16, 1, 1, nullptr);
    ImageList_AddMasked(g_hBtnImg, hBmpBlank, RGB(255, 0, 255));

    HBITMAP hBmpRight = (HBITMAP)LoadImageW(
        NULL,
        L"C:\\bmp\\Nuion_Arrow_Right2.bmp",
        IMAGE_BITMAP,
        16, 16,            // 원하는 크기가 있으면 지정 (0,0이면 원본 크기)
        LR_LOADFROMFILE
    );
    HBITMAP hBmpDown = (HBITMAP)LoadImageW(
        NULL,
        L"C:\\bmp\\Nuion_Arrow_Bottom2.bmp",
        IMAGE_BITMAP,
        16, 16,
        LR_LOADFROMFILE
    );

    ImageList_AddMasked(g_hBtnImg, hBmpRight, RGB(255, 0, 255)); // 인덱스 1
    ImageList_AddMasked(g_hBtnImg, hBmpDown, RGB(255, 0, 255)); // 인덱스 2

    // 2) 트리뷰에 State ImageList 설정
    TreeView_SetImageList(g_hTreeView, g_hBtnImg, TVSIL_STATE);

    TVINSERTSTRUCT tvis = {};
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
    tvis.item.stateMask = TVIS_STATEIMAGEMASK;

    // [1] 최상위 루트
    tvis.hParent = TVI_ROOT;
    tvis.item.pszText = (LPWSTR)L"최상위1";
    tvis.item.state = INDEXTOSTATEIMAGEMASK(1);     // 화살표 표시
    HTREEITEM hRoot = TreeView_InsertItem(g_hTreeView, &tvis);

    // [2] 루트의 두 자식
    const wchar_t* childNames[2] = { L"    자식 노드 A", L"자식 노드 B" };
    HTREEITEM children[2];

    tvis.hParent = hRoot;
    tvis.item.pszText = (LPWSTR)childNames[0];
    tvis.item.state = 0;     // 화살표 표시
    children[0] = TreeView_InsertItem(g_hTreeView, &tvis);

    tvis.hParent = hRoot;
    tvis.item.pszText = (LPWSTR)childNames[1];
    tvis.item.state = INDEXTOSTATEIMAGEMASK(1);     // 화살표 표시
    children[1] = TreeView_InsertItem(g_hTreeView, &tvis);

    // 3) 각 자식 아래에 두 개씩 손자 노드
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM;    // ← TVIF_STATE 빼기
    const wchar_t* grandNames[2] = { L"손자 노드 1", L"손자 노드 2" };

        for (int j = 0; j < 2; ++j) {
            tvis.hParent = children[1];
            tvis.item.state = 0;     // 화살표 없앰
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

    return (int)msg.wParam;
}


void testTreeItems(HWND hWndTree)
{

}