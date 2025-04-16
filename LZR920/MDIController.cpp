#include "pch.h"
#include "MDIController.h"
#include "MDIModel.h"
#include "MDIView.h"
#include "LZR920_Func.h"

CMDIController::CMDIController()
{
}

CMDIController::~CMDIController()
{
}

LRESULT CMDIController::MDIProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}

void CMDIController::createView(HWND parentWnd, int x, int y, int width, int height)
{
}

void CMDIController::destroyView()
{
}

bool CMDIController::isCreateView()
{
	return false;
}

RECT CMDIController::getViewRect()
{
	return RECT();
}
