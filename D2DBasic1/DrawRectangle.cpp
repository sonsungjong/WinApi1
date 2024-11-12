#include "DrawRectangle.h"

DemoApp::DemoApp()
	: m_hWnd(NULL)
	, m_pDirect2dFactory(NULL)
	, m_pRenderTarget(NULL)
	, m_pLightSlateGrayBrush(NULL)
	, m_pCornflowerBlueBrush(NULL)
{
}

DemoApp::~DemoApp()
{
	DiscardDeviceResources();
	SAFE_RELEASE(m_pDirect2dFactory);
}

HRESULT DemoApp::init(HINSTANCE hInstance)
{
	HRESULT hr = createDeviceIndependentResources();
	if (SUCCEEDED(hr)) {
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = hInstance;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"D2DDemoApp";
		RegisterClassEx(&wcex);

		m_hWnd = CreateWindow(L"D2DDemoApp", L"Direct2D Demo Application",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			640, 480, NULL, NULL, hInstance, this);
		hr = m_hWnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr)) {
			ShowWindow(m_hWnd, SW_SHOWNORMAL);
			UpdateWindow(m_hWnd);
		}
	}

	return hr;
}

void DemoApp::runMessageLoop()
{
	MSG msg = { };

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// render();

		}
	}
}

HRESULT DemoApp::createDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// 1. 팩토리 인터페이스 객체 생성 (싱글/멀티 쓰레드)
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	return hr;
}

HRESULT DemoApp::createDeviceResources()
{
	return E_NOTIMPL;
}

void DemoApp::DiscardDeviceResources()
{
}

HRESULT DemoApp::OnRender()
{
	return E_NOTIMPL;
}

void DemoApp::OnResize(UINT width, UINT height)
{
}

LRESULT DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}
