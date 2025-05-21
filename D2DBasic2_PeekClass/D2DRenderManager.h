#pragma once

typedef struct STRenderTargetInfo
{
	HWND hWnd;														// ��� ������
	ID2D1HwndRenderTarget* pRenderTarget;			// ���� ��� ���
	ID2D1BitmapRenderTarget* pBackBuffer;				// ����� �׸��� ���
	ID2D1Bitmap* pCopyBitmap;								// ����ۿ��� ����� ��Ʈ��
} ST_RenderTargetInfo;

class D2DRenderManager
{
public:
	static D2DRenderManager& getInstance();

	void init();
	void destroy();

	void addTargetWnd(HWND hWnd, const RECT& rc);
	void removeTargetWnd(HWND hWnd);

	ID2D1BitmapRenderTarget* getBackBuffer(HWND hWnd);
	void updateView(HWND hWnd);
	void updateViewAll();

private:
	ID2D1Factory* m_factory = NULL;
	std::unordered_map<HWND, ST_RenderTargetInfo> m_umapTargets;
};

