#pragma once

class CMDIModel;
class CMDIView;

class CMDIController
{
public:
	CMDIController();
	virtual ~CMDIController();

	CMDIModel* m_pMDIModel;
	CMDIView* m_pMDIView;
	HWND m_parentWnd;

	static LRESULT CALLBACK MDIProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void createView(HWND parentWnd, int x, int y, int width, int height);
	void destroyView();
	bool isCreateView();
	RECT getViewRect();
};

