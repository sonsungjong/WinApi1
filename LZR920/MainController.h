#pragma once

class CMainModel;
class CMainView;
class CMDIController;

class CMainController
{
public:
	CMainController();
	virtual ~CMainController();

	CMainModel* m_pMainModel;
	CMainView* m_pMainView;
	CMDIController* m_pMDIController;
	HWND m_parentWnd;

	static LRESULT CALLBACK MainProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void createView(HWND parentWnd, int x, int y, int width, int height);
	void destroyView();
	bool isCreateView();
	RECT getViewRect();
	

};

