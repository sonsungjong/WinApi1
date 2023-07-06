#pragma once

#include "resource.h"

class DlgClass
{
public:
	DlgClass() : m_hWnd(NULL) {}
	~DlgClass() {}

	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	INT_PTR CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void OnCommand(int id);
	void OnPaint();
	void OnDestroy();
	void OnInitialDialog();

	HWND m_hWnd;
	HFONT m_font;
	HDC m_hdc;

private:

};