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

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT DemoApp::createDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// 1. 팩토리 인터페이스 객체 생성 (싱글/멀티 쓰레드)
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
	//hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pDirect2dFactory);

	return hr;
}

HRESULT DemoApp::createDeviceResources()
{
	HRESULT hr = S_OK;
	if (!m_pRenderTarget) {
		::RECT rect;
		GetClientRect(m_hWnd, &rect);
		D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);

		hr = m_pDirect2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hWnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightSlateGray), &m_pLightSlateGrayBrush);
		}

		if (SUCCEEDED(hr)) {
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
		}
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pLightSlateGrayBrush);
	SAFE_RELEASE(m_pCornflowerBlueBrush);
}

HRESULT DemoApp::OnRender()
{
	HRESULT hr = S_OK;
	hr = createDeviceResources();
	if (SUCCEEDED(hr)) {
		// 렌더 타겟이 유효함
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		// 그리기 함수 호출
		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
		int width = static_cast<int>(rtSize.width);
		int height = static_cast<int>(rtSize.height);
		for (int x = 0; x < width; x += 10) {
			m_pRenderTarget->DrawLine(D2D1::Point2F(static_cast<FLOAT>(x), 0.0f), D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height), m_pLightSlateGrayBrush, 0.5f);
		}

		// 화면 중간에 두 사각형을 그림
		D2D1_RECT_F rect1 = D2D1::RectF(rtSize.width / 2 - 50.f, rtSize.height / 2 - 50.f, rtSize.width / 2 + 50.0f, rtSize.height / 2 + 50.0f);
		D2D1_RECT_F rect2 = D2D1::RectF(rtSize.width / 2 - 100.f, rtSize.height / 2 - 100.f, rtSize.width / 2 + 100.0f, rtSize.height / 2 + 100.0f);
		m_pRenderTarget->FillRectangle(&rect1, m_pLightSlateGrayBrush);
		m_pRenderTarget->DrawRectangle(&rect2, m_pCornflowerBlueBrush);

		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET) {
		// 렌더타겟을 재생성 해야함
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget != NULL) {
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

LRESULT DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE) {
		// WM_CREATE 메시지 처리에서 pDemoApp을 설정
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, PtrToUlong(pDemoApp));
		result = 1;
	}
	else {
		// pDemoApp을 안전하게 가져오기
		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hWnd, GWLP_USERDATA)));
		bool wasHandled = false;

		if (pDemoApp) {  // pDemoApp이 유효할 때만 처리
			switch (message) {
			case WM_SIZE:
				 { 
					UINT width = LOWORD(lParam);
					UINT height = HIWORD(lParam);
					pDemoApp->OnResize(width, height);
				}
				result = 0;
				wasHandled = true;
				break;
			case WM_DISPLAYCHANGE:
				InvalidateRect(hWnd, NULL, FALSE);
				result = 0;
				wasHandled = true;
				break;

			case WM_PAINT:
				pDemoApp->OnRender();
				ValidateRect(hWnd, NULL);
				result = 0;
				wasHandled = true;
				break;

			case WM_DESTROY:
				PostQuitMessage(0);
				result = 1;
				wasHandled = true;
				break;
			}
		}

		if (!wasHandled) {
			result = DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	return result;
}