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
    //    // â�� �巡���� �� �ְ� ����
    //    return HTCAPTION;
    //}
    //else if (WM_COMMAND == msg)
    //{
    //    int id = LOWORD(wParam);                        // ���ID
    //    HWND hwndCtrl = (HWND)lParam;           // ����� �߻���Ų ��Ʈ�� �ڵ� (������ NULL)
    //    UINT codeNotify = HIWORD(wParam);       // �˸� �ڵ�
    //    OnCommand(hWnd, id, hwndCtrl, codeNotify);
    //}
    //else if (WM_TIMER == msg)
    //{
    //    UINT_PTR timerID = (UINT_PTR)wParam;                    // Ÿ�̸� �ĺ���
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