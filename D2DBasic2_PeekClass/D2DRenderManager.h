#pragma once

typedef struct STRenderTargetInfo
{
	HWND hWnd;														// ��� ������
	ID2D1HwndRenderTarget* pRenderTarget;			// ���� ��� ���
	ID2D1BitmapRenderTarget* pBackBuffer;				// ����� �׸��� ���
	ID2D1Bitmap* pCopyBitmap;								// ����ۿ��� ����� ��Ʈ��
	std::function<void(ID2D1BitmapRenderTarget*)> drawCallback;
} ST_RenderTargetInfo;

class D2DRenderManager
{
public:
	static D2DRenderManager& getInstance();

	void init();
	void destroy();

	void addTargetWnd(HWND hWnd, std::function<void(ID2D1BitmapRenderTarget*)> drawFunc);
	void removeTargetWnd(HWND hWnd);

	ID2D1BitmapRenderTarget* getBackBuffer(HWND hWnd);
	void updateView(HWND hWnd);
	void updateViewAll();

	bool isEmptyMap() const;
private:
	ID2D1Factory* m_factory = NULL;
	std::unordered_map<HWND, ST_RenderTargetInfo> m_umapTargets;
};

