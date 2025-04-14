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
	memset(m_CB, 0, sizeof(m_CB));				// ������ �迭 �ʱ�ȭ
}

CDevice::~CDevice()
{
	SafeDelArray(m_CB);						// SafeDelArray<CConstBuffer, 4>(m_CB); �Լ����ø��� ���� �Լ��� ��������� ����

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
	// ���â�� ����׸޽��� ���
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, iFlag, nullptr, 0, D3D11_SDK_VERSION,
		&m_device, &level, &m_context)))
	{
		return E_FAIL;
	}

	// SwapChain ����
	if (!(SUCCEEDED(createSwapChain())))
	{
		return E_FAIL;
	}

	// View ����
	if (!(SUCCEEDED(createView()))) {
		return E_FAIL;
	}

	// ViewPort ���� : ������ ȭ�鿡 ������ ������ ����
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(m_render_resolution.x);
	viewport.Height = static_cast<FLOAT>(m_render_resolution.y);
	// ���� �ؽ��Ŀ� ����Ǵ� ���� Min, Max ����
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	// ViewPort ���� ����
	m_context->RSSetViewports(1, &viewport);

	// �ʿ��� ������� �̸� ����
	if (!(SUCCEEDED(createConstBuffer())))
	{
		return E_FAIL;
	}

	if (!(SUCCEEDED(createSamplerState()))) 
	{
		return E_FAIL;
	}

	// Rasterizer State ����
	if (!(SUCCEEDED(createRasterizerState()))) 
	{
		return E_FAIL;
	}

	// DepthStencilState ����
	if (FAILED(createDepthStencilState()))
	{
		return E_FAIL;
	}

	// BlendState ����
	if (FAILED(createBlendState()))
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

	desc.OutputWindow = m_hWnd;				// ��� ������ �ڵ鰪
	desc.Windowed = true;

	// ����ʹ� ���� 60Hz ~ 144Hz �� ȭ���� �����Ѵ�
	desc.BufferCount = 1;									// ����۸� �Ѱ��� ����� (���� FLIP���� �����Ϸ��� 2�� �̻�)
	desc.BufferDesc.Width = static_cast<UINT>(m_render_resolution.x);
	desc.BufferDesc.Height = static_cast<UINT>(m_render_resolution.y);
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	desc.BufferDesc.RefreshRate.Denominator = 1;				// ȭ�� ���� ����
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;					// ����� �������� ������ ���� ���� BITBLT ���

	// ComPtr<IDXGIDevice> �� ����ϸ� �ʱ�ȭ �� ����� �ʿ����
	// ���� �����ʹ� .Get() ����, ���������ʹ� .GetAddressOf() �� ��ȯ
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
	// DX11 ������ 0�� ���۰� ���� ����� �����
	// 1. RenderTarget Texture �� 0�� ����ü�����κ��� ��������
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_renderTargetTex);
	// 2. RenderTargetView�� �����Ѵ�
	m_device->CreateRenderTargetView(m_renderTargetTex, nullptr, &m_RTV);

	// 3. DepthStencil �� Texture ����
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

	// RenderTarget�� DepthStencilTarget�� ������� ����
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

	desc[0].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;				// �ʰ��ϸ� �ݺ��������� ���
	desc[0].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[0].AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[0].Filter = D3D11_FILTER_ANISOTROPIC;
	CDevice::getInstance()->GetDivice()->CreateSamplerState(desc, m_arrSamplerState[0].GetAddressOf());
	CDevice::getInstance()->GetContext()->PSSetSamplers(0, 1, m_arrSamplerState[0].GetAddressOf());						// s0

	desc[1].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[1].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[1].AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc[1].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;					// ������ �������� �״�� (���ػ� �̹��� ����� �� ��պ����� �ƴ϶� �״�� ����ؼ� ���)
	CDevice::getInstance()->GetDivice()->CreateSamplerState(desc + 1, m_arrSamplerState[1].GetAddressOf());
	CDevice::getInstance()->GetContext()->PSSetSamplers(1, 1, m_arrSamplerState[1].GetAddressOf());						// s1

	return 0;
}

int CDevice::createRasterizerState()
{
	// ���� ����ü�� ä��� create ��Ų��
	D3D11_RASTERIZER_DESC desc = {};

	// [0] CULL_BACK ����� Default �ɼ��̱� ������, nullptr �� �д� (BACK(�ݽð����)���� ������ ���� �ɷ�����)
	m_arrRasterizerState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

	// [1] CULL_FRONT
	desc.CullMode = D3D11_CULL_FRONT;					// FRONT(�ð����) �� ������ ���� �ɷ�����
	desc.FillMode = D3D11_FILL_SOLID;
	CDevice::getInstance()->GetDivice()->CreateRasterizerState(&desc, m_arrRasterizerState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());
	
	// [2] CULL_NONE
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_SOLID;
	CDevice::getInstance()->GetDivice()->CreateRasterizerState(&desc, m_arrRasterizerState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

	// [3] WIRE_FRAME
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_WIREFRAME;
	CDevice::getInstance()->GetDivice()->CreateRasterizerState(&desc, m_arrRasterizerState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());

	return S_OK;
}

int CDevice::createDepthStencilState()
{
	m_DS[(UINT)DS_TYPE::LESS] = nullptr;				// �⺻�� nullptr

	D3D11_DEPTH_STENCIL_DESC desc = {};

	// LESS_EQUAL
	desc.DepthEnable = true;
	desc.StencilEnable = false;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;					// �۰ų� ���� ��� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;				// ���� ���

	CDevice::getInstance()->GetDivice()->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());

	// GREATER
	desc.DepthEnable = true;
	desc.StencilEnable = false;
	desc.DepthFunc = D3D11_COMPARISON_GREATER;					// ū ��� ���
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;				// ���� ���

	CDevice::getInstance()->GetDivice()->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::GREATER].GetAddressOf());

	// NO_TEST
	desc.DepthEnable = true;
	desc.StencilEnable = false;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;					// 
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;				// ���� ���

	CDevice::getInstance()->GetDivice()->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::NO_TEST].GetAddressOf());

	// NO_TEST + NO_WRITE
	desc.DepthEnable = false;									// ���� ����� ����
	desc.StencilEnable = false;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;					// 
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;				// ���� ��� ����

	CDevice::getInstance()->GetDivice()->CreateDepthStencilState(&desc, m_DS[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());

	return S_OK;
}

int CDevice::createBlendState()
{
	m_BS[(UINT)BS_TYPE::DEFAULT] = nullptr;								// �⺻ �ɼ��� nullptr (���ĸ� ������)

	D3D11_BLEND_DESC Desc = {};

	// ALPHA_BLEND
	Desc.AlphaToCoverageEnable = true;					// ���� ������ �����ʰ� ���ش�
	Desc.IndependentBlendEnable = false;					// false�� �ϸ� RenderTarget�� 8�� �� ó�� �͸� ����ϰ� �ȴ�
	Desc.RenderTarget[0].BlendEnable = true;
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	HRESULT hr = CDevice::getInstance()->GetDivice()->CreateBlendState(&Desc, m_BS[(UINT)BS_TYPE::ALPHA_BLEND].GetAddressOf());

	// ONE_ONE
	Desc.AlphaToCoverageEnable = false;
	Desc.IndependentBlendEnable = false;					// false�� �ϸ� RenderTarget�� 8�� �� ó�� �͸� ����ϰ� �ȴ�
	Desc.RenderTarget[0].BlendEnable = true;
	Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	hr = CDevice::getInstance()->GetDivice()->CreateBlendState(&Desc, m_BS[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());

	return S_OK;
}
