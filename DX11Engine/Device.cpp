#include "pch.h"
#include "Device.h"

CDevice::CDevice()
	: m_hWnd(nullptr)
	, m_render_resolution{}
	, m_device(nullptr)
	, m_context(nullptr)
	, m_swapChain(nullptr)
	, m_renderTargetTex(nullptr)
	, m_RTV(nullptr)
	, m_depthStencilTex(nullptr)
	, m_DSV(nullptr)
{

}

CDevice::~CDevice()
{
	m_device->Release();
	m_context->Release();
}

int CDevice::init(HWND _hWnd, POINT _resolution)
{
	m_hWnd = _hWnd;
	m_render_resolution = _resolution;

	UINT iFlag = 0U;
	D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;

#ifdef _DEBUG
	// 출력창에 디버그메시지 출력
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, iFlag, nullptr, 0, D3D11_SDK_VERSION, 
		&m_device, &level, &m_context);

	return S_OK;
}