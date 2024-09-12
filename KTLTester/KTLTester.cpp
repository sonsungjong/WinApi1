#include "pch.h"
#include "KTLTester.h"
#include "DlgClass.h"

#ifdef _DEBUG
#pragma comment(lib, "AccelerationCoefficientLibD.lib")
#else
#pragma comment(lib, "AccelerationCoefficientLib.lib")
#endif

HINSTANCE hInst;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    hInst = hInstance;
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgClass::DlgProc);

    return 0;
}