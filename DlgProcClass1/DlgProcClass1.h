#pragma once

#include "resource.h"

class DlgClass
{
public:
	explicit DlgClass();
	virtual ~DlgClass();

	void OnCommand(int id);
	void OnPaint();
	void OnDestroy();
	void OnInitialDialog();

	HWND m_hWnd;
	HINSTANCE m_hInst;
private:

};