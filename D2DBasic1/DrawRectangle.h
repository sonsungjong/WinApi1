#pragma once

#define SAFE_RELEASE(p) {if(p){(p)->Release(); (p)=NULL;}}

#include <Windows.h>
#include <d2d1.h>       // Direct2D 기본 헤더
#include <dwrite.h>     // DirectWrite 헤더 (텍스트 렌더링이 필요한 경우)
#include <wincodec.h>   // WIC 헤더 (이미지 로딩을 위한 필수 요소)

class DemoApp
{
public:
	DemoApp();
	~DemoApp();
	HRESULT init(HINSTANCE hInstance);
	void runMessageLoop();
	HWND m_hWnd;

private:
	HRESULT createDeviceIndependentResources();
	HRESULT createDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
	ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
};

