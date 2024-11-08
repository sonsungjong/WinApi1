#include "Dx11Base.h"

CDx11Base::CDx11Base()
{
	m_hWnd = NULL;
	m_hInst = NULL;
	m_pD3DDevice = NULL;
	m_pD3DContext = NULL;
	m_pD3DRenderTargetView = NULL;
	m_pSwapChain = NULL;
}

CDx11Base::~CDx11Base()
{
}

bool CDx11Base::Initialize(HWND hWnd, HINSTANCE hInst)
{
	bool func_result = false;
	m_hWnd = hWnd;
	m_hInst = hInst;

	::RECT rc;
	::GetClientRect(hWnd, &rc);
	unsigned int nWidth = rc.right = rc.left;
	unsigned int nHeight = rc.bottom - rc.top;

	// Swap chain structure
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	memset(&swapChainDesc, 0, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = nWidth;
	swapChainDesc.BufferDesc.Height = nHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = (HWND)hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;

	// 레벨
	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	unsigned int numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE, D3D_DRIVER_TYPE_SOFTWARE
	};
	unsigned int numDriverTypes = ARRAYSIZE(driverTypes);

	unsigned int flags = 0U;
#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// D3D디바이스와 swap chain을 생성한다
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		featureLevels,
		numFeatureLevels,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&m_pSwapChain,
		&m_pD3DDevice,
		&featureLevel,
		&m_pD3DContext
	);

	if (SUCCEEDED(hr)) {
		ID3D11Texture2D* pBackBuffer;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (SUCCEEDED(hr)) {
			hr = m_pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pD3DRenderTargetView);

			if (pBackBuffer != NULL) {
				pBackBuffer->Release();
			}

			if (SUCCEEDED(hr)) {
				// set the render target
				m_pD3DContext->OMSetRenderTargets(1, &m_pD3DRenderTargetView, NULL);

				// Set the viewport
				D3D11_VIEWPORT viewPort;
				viewPort.Width = static_cast<float>(nWidth);
				viewPort.Height = static_cast<float>(nHeight);
				viewPort.MinDepth = 0.0f;
				viewPort.MaxDepth = 1.0f;
				viewPort.TopLeftX = 0;
				viewPort.TopLeftY = 0;
				m_pD3DContext->RSSetViewports(1, &viewPort);

			    func_result = LoadContent();
			}
			else {
				::MessageBox(hWnd, TEXT("Unable to create render target view"), TEXT("EROR"), MB_OK);
			}
		}
		else {
			::MessageBox(hWnd, TEXT("Unable to get back buffer"), TEXT("EROR"), MB_OK);
		}
	}
	else {
		::MessageBox(hWnd, TEXT("A DX11 Video Card is Required"), TEXT("ERROR"), MB_OK);
	}

	return func_result;
}

void CDx11Base::Terminate()
{
	// 오버라이딩
	UnloadContent();

	// cleanup
	if (m_pD3DRenderTargetView != NULL) {
		m_pD3DRenderTargetView->Release();
		m_pD3DRenderTargetView = NULL;
	}
	if (m_pSwapChain != NULL) {
		m_pSwapChain->Release();
		m_pSwapChain = NULL;
	}
	if (m_pD3DContext != NULL) {
		m_pD3DContext->Release();
		m_pD3DContext = NULL;
	}
	if (m_pD3DDevice != NULL) {
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
}
