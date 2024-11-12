#include "pch.h"
#include "Engine.h"

#include "Device.h"

// 정적 멤버는 별도로 초기화를 해줘야한다
CEngine* CEngine::g_this = nullptr;

// 윈도우와 해상도
int CEngine::init(HWND _hWnd, POINT _resolution)
{
	m_hWnd = _hWnd;							// 대상윈도우
	m_resolution = _resolution;				// 해상도

	::RECT rt = { 0, 0, m_resolution.x, m_resolution.y };

	// 윈도우 크기 설정
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);								// 메뉴는 false
	SetWindowPos(m_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

	// DirectX11
	if (!SUCCEEDED(CDevice::getInstance()->init(m_hWnd, m_resolution))) 
	{
		::MessageBox(m_hWnd, L"Device 초기화 실패", L"엔진 초기화 실패", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void CEngine::progress()
{
	// Level->tick();								// 한틱에 수행할 행동

	float clear_color[4] = { 0.3f, 0.3f, 0.3f, 1.f };
	CDevice::getInstance()->clearTarget(clear_color);			// 모두 지우고

	// Level->render();							// 렌더타겟에 그린다

	// SwapChain->Present();				// 윈도우 화면에 보낸다
	CDevice::getInstance()->present();
}

CEngine::CEngine()
	: m_hWnd(nullptr)
	, m_resolution{}
{

}

CEngine::~CEngine()
{
}
