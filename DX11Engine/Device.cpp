#include "pch.h"
#include "Device.h"

#include "ConstBuffer.h"

CDevice::CDevice()
	: m_hWnd(nullptr)
	, m_arrSamplerState{}
	, m_device(nullptr)
	, m_context(nullptr)
	, m_swapChain(nullptr)
	, m_renderTargetTex(nullptr)
	, m_RTV(nullptr)
	, m_depthStencilTex(nullptr)
	, m_DSV(nullptr)
{
	memset(m_CB, 0, sizeof(m_CB));				// 포인터 배열 초기화
}

CDevice::~CDevice()
{
	SafeDelArray(m_CB);						// SafeDelArray<CConstBuffer, 4>(m_CB); 함수템플릿을 통해 함수가 만들어져서 동작

	m_device->Release();
	m_context->Release();
	m_swapChain->Release();
	m_renderTargetTex->Release();
	m_RTV->Release();
	m_depthStencilTex->Release();
	m_DSV->Release();
}

int CDevice::init(HWND _hWnd, Vec2 _resolution)
{
	m_hWnd = _hWnd;
	m_render_resolution = _resolution;

	UINT iFlag = 0U;
	D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;

#ifdef _DEBUG
	// 출력창에 디버그메시지 출력
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, iFlag, nullptr, 0, D3D11_SDK_VERSION,
		&m_device, &level, &m_context)))
	{
		return E_FAIL;
	}

	// SwapChain 생성
	if (!(SUCCEEDED(createSwapChain())))
	{
		return E_FAIL;
	}

	// View 생성
	if (!(SUCCEEDED(createView()))) {
		return E_FAIL;
	}

	// ViewPort 생성 : 윈도우 화면에 보여질 영역을 설정
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(m_render_resolution.x);
	viewport.Height = static_cast<FLOAT>(m_render_resolution.y);
	// 깊이 텍스쳐에 저장되는 깊이 Min, Max 지정
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	// ViewPort 정보 세팅
	m_context->RSSetViewports(1, &viewport);

	// 필요한 상수버퍼 미리 생성
	if (!(SUCCEEDED(createConstBuffer())))
	{
		return E_FAIL;
	}

	if (!(SUCCEEDED(createSamplerState()))) 
	{
		return E_FAIL;
	}

	// Rasterizer State 생성
	if (!(SUCCEEDED(createRasterizerState()))) 
	{
		return E_FAIL;
	}

	return S_OK;
}

void CDevice::clearTarget(float(&_ArrColor)[4])
{
	m_context->ClearRenderTargetView(m_RTV, _ArrColor);

	m_context->ClearDepthStencilView(m_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void CDevice::present()
{
	m_swapChain->Present(0, 0);
}

int CDevice::createSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc = {};

	desc.OutputWindow = m_hWnd;				// 출력 윈도우 핸들값
	desc.Windowed = true;

	// 모니터는 보통 60Hz ~ 144Hz 로 화면을 갱신한다
	desc.BufferCount = 1;									// 백버퍼를 한개로 만든다 (참조 FLIP으로 전달하려면 2개 이상)
	desc.BufferDesc.Width = static_cast<UINT>(m_render_resolution.x);
	desc.BufferDesc.Height = static_cast<UINT>(m_render_resolution.y);
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	desc.BufferDesc.RefreshRate.Denominator = 1;				// 화면 갱신 비율
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;					// 모니터 독립적인 프레임 제한 없는 BITBLT 방식

	// ComPtr<IDXGIDevice> 를 사용하면 초기화 및 릴리즈가 필요없음
	// 단일 포인터는 .Get() 으로, 이중포인터는 .GetAddressOf() 로 반환
	IDXGIDevice* pDXGIDevice = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIFactory* pFactory = nullptr;

	m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	if (FAILED(pFactory->CreateSwapChain(m_device, &desc, &m_swapChain))) {
		return E_FAIL;
	}

	pDXGIDevice->Release();
	pAdapter->Release();
	pFactory->Release();

	return S_OK;
}

int CDevice::createView()
{
	// DX11 까지는 0번 버퍼가 다음 사용할 백버퍼
	// 1. RenderTarget Texture 를 0번 스왚체인으로부터 가져오기
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_renderTargetTex);
	// 2. RenderTargetView를 생성한다
	m_device->CreateRenderTargetView(m_renderTargetTex, nullptr, &m_RTV);

	// 3. DepthStencil 용 Texture 제작
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.Width = static_cast<UINT>(m_render_resolution.x);
	desc.Height = static_cast<UINT>(m_render_resolution.y);
	desc.ArraySize = 1;
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	m_device->CreateTexture2D(&desc, nullptr, &m_depthStencilTex);
	// 4. DepthStencil View
	m_device->CreateDepthStencilView(m_depthStencilTex, nullptr, &m_DSV);

	// RenderTarget과 DepthStencilTarget을 출력으로 지정
	m_context->OMSetRenderTargets(1, &m_RTV, m_DSV);

	return S_OK;
}

int CDevice::createConstBuffer()
{
	m_CB[(UINT)CB_TYPE::TRANSFORM] = new CConstBuffer;
	m_CB[(UINT)CB_TYPE::TRANSFORM]->create(sizeof(tTransform), CB_TYPE::TRANSFORM);

	m_CB[(UINT)CB_TYPE::MATERIAL] = new CConstBuffer;
	m_CB[(UINT)CB_TYPE::MATERIAL]->create(sizeof(tMtrlConst), CB_TYPE::MATERIAL);

	return S_OK;
}

int CDevice::createSamplerState()
{
	D3D11_SAMPLER_DESC desc[2] = {};

	desc[0].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;				// 초과하면 반복패턴으로 출력
	desc[0].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[0].AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[0].Filter = D3D11_FILTER_ANISOTROPIC;
	DEVICE->CreateSamplerState(desc, m_arrSamplerState[0].GetAddressOf());
	CONTEXT->PSSetSamplers(0, 1, m_arrSamplerState[0].GetAddressOf());						// s0

	desc[1].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[1].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[1].AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[1].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;					// 색상을 변형없이 그대로 (저해상도 이미지 출력할 때 평균보정이 아니라 그대로 배수해서 출력)
	DEVICE->CreateSamplerState(desc + 1, m_arrSamplerState[1].GetAddressOf());
	CONTEXT->PSSetSamplers(1, 1, m_arrSamplerState[1].GetAddressOf());						// s1

	return 0;
}

int CDevice::createRasterizerState()
{
	// 관련 구조체를 채우고 create 시킨다
	D3D11_RASTERIZER_DESC desc = {};

	// [0] CULL_BACK 기능은 Default 옵션이기 때문에, nullptr 로 둔다 (BACK(반시계방향)으로 구성된 면은 걸러낸다)
	m_arrRasterizerState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

	// [1] CULL_FRONT
	desc.CullMode = D3D11_CULL_FRONT;					// FRONT(시계방향) 로 구성된 면은 걸러낸다
	desc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&desc, m_arrRasterizerState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());
	
	// [2] CULL_NONE
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_SOLID;
	DEVICE->CreateRasterizerState(&desc, m_arrRasterizerState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

	// [3] WIRE_FRAME
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_WIREFRAME;
	DEVICE->CreateRasterizerState(&desc, m_arrRasterizerState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());

	return S_OK;
}
