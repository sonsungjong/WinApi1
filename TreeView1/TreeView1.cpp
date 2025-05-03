#include "framework.h"
#include "TreeView1.h"
//#include <uxtheme.h>

//#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

HINSTANCE g_hInst;
HWND g_hWnd;
ULONG_PTR g_gdiplusToken = 0;

HIMAGELIST g_hBtnImg;
HIMAGELIST g_hStateCircleImg;

HWND g_hTreeView;
CTreeData g_treeData;
ST_TreeNode* rootA;
ST_TreeNode* gp_rootA_nodeA;
ST_TreeNode* gp_rootA_nodeB;
ST_TreeNode* gp_rootA_nodeC;
ST_TreeNode* gp_rootA_nodeB_nodeA;
ST_TreeNode* gp_rootA_nodeB_nodeB;

static int tester = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_NOTIFY:
    {
        LPNMHDR pnm = (LPNMHDR)lParam;
        HWND hTree = pnm->hwndFrom;
        //if (pnm->idFrom == IDC_MY_TREE)
        {
            if (pnm->code == NM_DBLCLK)
            {
                // 더블클릭 막기
                return TRUE;
            }
            else if (pnm->code == NM_CLICK)
            {
                DWORD pos = GetMessagePos();
                POINT pt = { GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };
                ScreenToClient(hTree, &pt);

                TVHITTESTINFO hti = { pt };
                HTREEITEM hItem = TreeView_HitTest(hTree, &hti);

                const UINT mask = TVHT_ONITEMSTATEICON | TVHT_ONITEMLABEL;
                if (hItem && (hti.flags & mask))
                {
                    UINT state = TreeView_GetItemState(hTree, hItem, TVIS_STATEIMAGEMASK);
                    int idx = state >> 12;  // high nibble

                    if (idx == 0) {
                        return 0;
                    }

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

                    TVITEMW ti = {};
                    ti.hItem = hItem;
                    ti.mask = TVIF_STATE;
                    ti.stateMask = TVIS_STATEIMAGEMASK;
                    ti.state = INDEXTOSTATEIMAGEMASK(next);
                    TreeView_SetItem(hTree, &ti);

                    TreeView_Expand(hTree, hItem, next == 3 ? TVE_EXPAND : TVE_COLLAPSE );
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
                    pTvcd->clrTextBk = GetSysColor(COLOR_WINDOW);
                    pTvcd->clrText = GetSysColor(COLOR_WINDOWTEXT);
                    return CDRF_NEWFONT;

                    //return CDRF_NOTIFYPOSTPAINT;
                }
                else if (stage == CDDS_ITEMPOSTPAINT)
                {

                    return CDRF_SKIPDEFAULT;
                }
                return CDRF_DODEFAULT;
            }
            else if (pnm->code == TVN_KEYDOWN)
            {
                tester++;
                tester %= 4;
                TREE_ICON_COLOR_INDEX color_test = static_cast<TREE_ICON_COLOR_INDEX>(tester);

                //g_treeData.SetNodeIconColor(gp_rootA_nodeB, TREE_ICON_COLOR_INDEX::RED);
                //g_treeData.SetNodeIconColor(gp_rootA_nodeB_nodeA, TREE_ICON_COLOR_INDEX::GREEN);
                //g_treeData.SetNodeIconColor(gp_rootA_nodeB_nodeB, TREE_ICON_COLOR_INDEX::YELLOW);
                g_treeData.SetNodeIconColor(gp_rootA_nodeB, color_test);
                g_treeData.SetNodeIconColor(gp_rootA_nodeB_nodeA, color_test);
                g_treeData.SetNodeIconColor(gp_rootA_nodeB_nodeB, color_test);
                g_treeData.SetNodeText(gp_rootA_nodeB_nodeB, L"변경됨");
                
                g_treeData.RefreshView(g_hTreeView);
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


    // 1) State용 ImageList 생성 (예: 오른쪽 화살표→인덱스1, 아래 화살표→인덱스2)
    const int bmp_size = 24;
    g_hBtnImg = ImageList_Create(bmp_size, bmp_size, ILC_COLOR32 | ILC_MASK, 4, 1);

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

    // CIRCLE : gray(0), red(1), yellow(2), green(3)
    g_hStateCircleImg = ImageList_Create(bmp_size, bmp_size, ILC_COLOR32 | ILC_MASK, 4, 1);
    HBITMAP bmpGray = createGdiPlusCircleBitmap(bmp_size, RGB(129, 129, 129));
    HBITMAP bmpRed = createGdiPlusCircleBitmap(bmp_size, RGB(255, 0, 0));
    HBITMAP bmpYellow = createGdiPlusCircleBitmap(bmp_size, RGB(192, 192, 101));
    HBITMAP bmpGreen = createGdiPlusCircleBitmap(bmp_size, RGB(0, 172, 0));

    ImageList_AddMasked(g_hStateCircleImg, bmpGray, RGB(255, 0, 255));
    ImageList_AddMasked(g_hStateCircleImg, bmpRed, RGB(255, 0, 255));
    ImageList_AddMasked(g_hStateCircleImg, bmpYellow, RGB(255, 0, 255));
    ImageList_AddMasked(g_hStateCircleImg, bmpGreen, RGB(255, 0, 255));



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

    // 2) 트리뷰에 State ImageList 설정
    TreeView_SetImageList(g_hTreeView, g_hBtnImg, TVSIL_STATE);

    TreeView_SetImageList(g_hTreeView, g_hStateCircleImg, TVSIL_NORMAL);

    // 트리뷰 자료구조 채우기
    rootA = g_treeData.AddRoot(L"최상위1", static_cast<int>(TREE_ARROW_INDEX::CLOSE), static_cast<int>(TREE_ICON_COLOR_INDEX::GREY));
    // 루트의 자식
    gp_rootA_nodeA = g_treeData.AddChild(rootA, L"자식 노드 A", static_cast<int>(TREE_ARROW_INDEX::NONE), static_cast<int>(TREE_ICON_COLOR_INDEX::GREY));
    gp_rootA_nodeB = g_treeData.AddChild(rootA, L"자식 노드 B", static_cast<int>(TREE_ARROW_INDEX::CLOSE), static_cast<int>(TREE_ICON_COLOR_INDEX::GREY));
    gp_rootA_nodeC = g_treeData.AddChild(rootA, L"자식 노드 C", static_cast<int>(TREE_ARROW_INDEX::NONE), static_cast<int>(TREE_ICON_COLOR_INDEX::GREY));
    // B의 손자
    gp_rootA_nodeB_nodeA = g_treeData.AddChild(gp_rootA_nodeB, L"손자 노드 1", static_cast<int>(TREE_ARROW_INDEX::NONE), static_cast<int>(TREE_ICON_COLOR_INDEX::GREY));
    gp_rootA_nodeB_nodeB = g_treeData.AddChild(gp_rootA_nodeB, L"손자 노드 2", static_cast<int>(TREE_ARROW_INDEX::NONE), static_cast<int>(TREE_ICON_COLOR_INDEX::GREY));

    // 트리뷰에 반영
    g_treeData.PopulateTreeView(g_hTreeView);

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

// GDI+로 원 비트맵 생성
// circle_size: 비트맵 크기(px)
// fillColor : 원을 채울 RGB 색상
HBITMAP createGdiPlusCircleBitmap(int circle_size, COLORREF fillColor)
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

void CTreeData::PopulateTreeView(HWND hTree)
{
    // 리드로우 중지
    SendMessage(hTree, WM_SETREDRAW, FALSE, 0);

    // 항목 삭제
    TreeView_DeleteAllItems(hTree);

    // 항목 삽입
    for (auto& root : roots)
    {
        InsertRecursive(hTree, TVI_ROOT, root.get());
    }

    // 리드로우 재개
    SendMessage(hTree, WM_SETREDRAW, TRUE, 0);
    //InvalidateRect(hTree, NULL, FALSE);
    //UpdateWindow(hTree);
}

void CTreeData::RefreshView(HWND hTree)
{
    // 리드로우 중지
    SendMessage(hTree, WM_SETREDRAW, FALSE, 0);

    std::function<void(HTREEITEM)> recurse = [&](HTREEITEM hItem) {
        for (HTREEITEM h = hItem; h; h = TreeView_GetNextSibling(hTree, h)) {
            // 1) lParam으로 모델 노드 가져오기
            TVITEMW tv = { 0 };
            tv.hItem = h;
            tv.mask = TVIF_PARAM;
            TreeView_GetItem(hTree, &tv);
            ST_TreeNode* node = reinterpret_cast<ST_TreeNode*>(tv.lParam);
            if (node) {
                // 2) 모델의 iconColorIndex로 아이콘만 변경
                TVITEMW u = { 0 };
                u.hItem = h;
                u.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                u.pszText = const_cast<LPWSTR>(node->text.c_str());
                u.iImage = node->iconColorIndex;
                u.iSelectedImage = node->iconColorIndex;
                TreeView_SetItem(hTree, &u);
            }
            // 3) 자식으로 재귀
            if (HTREEITEM ch = TreeView_GetChild(hTree, h)) {
                recurse(ch);

            }
        }
    };

    if (HTREEITEM root = TreeView_GetRoot(hTree))
    {
        recurse(root);
    }

    // 리드로우 재개
    SendMessage(hTree, WM_SETREDRAW, TRUE, 0);
    //InvalidateRect(hTree, NULL, FALSE);
    //UpdateWindow(hTree);
}

ST_TreeNode* CTreeData::AddRoot(const std::wstring& txt, int state, int icon_color)
{
    roots.emplace_back(std::make_unique<ST_TreeNode>(txt, state, icon_color));
    return roots.back().get();
}

ST_TreeNode* CTreeData::AddChild(ST_TreeNode* parent, const std::wstring& txt, int state, int icon_color)
{
    auto node = std::make_unique<ST_TreeNode>(txt, state, icon_color);
    ST_TreeNode* ptr = node.get();
    parent->children.push_back(ptr);
    allNodes.emplace_back(std::move(node));

    return ptr;
}

void CTreeData::SetNodeIconColor(ST_TreeNode* node, TREE_ICON_COLOR_INDEX color)
{
    if (node)
    {
        node->iconColorIndex = static_cast<int>(color);
    }
}

void CTreeData::SetNodeText(ST_TreeNode* node, const std::wstring& newText)
{
    if (node) {
        node->text = newText;
    }
}

void CTreeData::CycleAllIconColors()
{
    std::function<void(ST_TreeNode*)> recurse = [&](ST_TreeNode* node) {
        node->iconColorIndex = (node->iconColorIndex + 1) % 4;
        for (ST_TreeNode* child : node->children)
            recurse(child);
        };

    // roots 벡터에 담긴 최상위 노드부터 재귀 시작
    for (auto& rootPtr : roots) {
        recurse(rootPtr.get());
    }
}

void CTreeData::InsertRecursive(HWND hTree, HTREEITEM hParent, ST_TreeNode* node)
{
    TVINSERTSTRUCTW tvis = {};
    tvis.hParent = hParent;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvis.item.lParam = reinterpret_cast<LPARAM>(node);
    tvis.item.pszText = const_cast<LPWSTR>(node->text.c_str());
    tvis.item.stateMask = TVIS_STATEIMAGEMASK;
    tvis.item.state = INDEXTOSTATEIMAGEMASK(node->stateIndex);
    tvis.item.iImage = node->iconColorIndex;
    tvis.item.iSelectedImage = node->iconColorIndex;

    HTREEITEM hItem = TreeView_InsertItem(hTree, &tvis);
    for (ST_TreeNode* child : node->children)
    {
        InsertRecursive(hTree, hItem, child);
    }
}
