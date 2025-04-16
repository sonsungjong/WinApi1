#include "pch.h"
#include "MainController.h"
#include "MainModel.h"
#include "MainView.h"
#include "LZR920_Func.h"
#include "MDIController.h"

CMainController::CMainController()
	: m_pMainModel(NULL)
	, m_pMainView(NULL)
	, m_pMDIController(NULL)
	, m_parentWnd(NULL)
{
}

CMainController::~CMainController()
{
}


void CMainController::createView(HWND parentWnd, int x, int y, int width, int height)
{
}

void CMainController::destroyView()
{
}

bool CMainController::isCreateView()
{
	return false;
}

RECT CMainController::getViewRect()
{
	return RECT();
}

LRESULT CMainController::MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //if (WM_PAINT == msg)
    //{
    //    OnPaint(hWnd);
    //}
    //else if (WM_NCHITTEST == msg)
    //{
    //    // 창을 드래그할 수 있게 변경
    //    return HTCAPTION;
    //}
    //else if (WM_COMMAND == msg)
    //{
    //    int id = LOWORD(wParam);                        // 명령ID
    //    HWND hwndCtrl = (HWND)lParam;           // 명령을 발생시킨 컨트롤 핸들 (없으면 NULL)
    //    UINT codeNotify = HIWORD(wParam);       // 알림 코드
    //    OnCommand(hWnd, id, hwndCtrl, codeNotify);
    //}
    //else if (WM_TIMER == msg)
    //{
    //    UINT_PTR timerID = (UINT_PTR)wParam;                    // 타이머 식별자
    //    OnTimer(hWnd, timerID);
    //}
    //else if (WM_CLOSE == msg)
    //{
    //    OnClose(hWnd);
    //}
    //else if (WM_DESTROY == msg)
    //{
    //    OnDestroy(hWnd);
    //}
    //else if (WM_CREATE == msg)
    //{
    //    OnCreate(hWnd, (LPCREATESTRUCT)lParam);
    //}
    //else
    //{
    //    return DefWindowProc(hWnd, msg, wParam, lParam);
    //}

    return 0;
}