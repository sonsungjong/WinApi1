#include "pch.h"
#include "Engine.h"

#include "Device.h"
#include "TimeMgr.h"
#include "PathMgr.h"
#include "KeyMgr.h"
#include "AssetMgr.h"
#include "LevelMgr.h"
#include "RenderMgr.h"


// 정적 멤버는 별도로 초기화를 해줘야한다
//CEngine* CEngine::g_this = nullptr;

// 윈도우와 해상도
int CEngine::init(HWND _hWnd, Vec2 _resolution)
{
	m_hWnd = _hWnd;							// 대상윈도우
	m_resolution = _resolution;				// 해상도

	::RECT rt = { 0, 0, static_cast<LONG>(m_resolution.x), static_cast<LONG>(m_resolution.y) };

	// 윈도우 크기 설정
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);								// 메뉴는 false
	SetWindowPos(m_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

	// DirectX11
	if (!SUCCEEDED(CDevice::getInstance()->init(m_hWnd, m_resolution))) 
	{
		::MessageBox(m_hWnd, L"Device 초기화 실패", L"엔진 초기화 실패", MB_OK);
		return E_FAIL;
	}

	// Manager Init
	CPathMgr::getInstance()->init();
	CTimeMgr::getInstance()->init();
	CKeyMgr::getInstance()->init();
	CAssetMgr::getInstance()->init();
	CLevelMgr::getInstance()->init();
	CRenderMgr::getInstance()->init();

	//if (FAILED(TempInit())) {
	//	MessageBox(m_hWnd, L"Device 초기화 실패", L"Temp Init 초기화 실패", MB_OK);
	//	return E_FAIL;
	//}

	return S_OK;
}

void CEngine::progress()
{
	// Level->tick();								// 한틱에 수행할 행동
	// Manager Tick
	CTimeMgr::getInstance()->tick();				// 한프레임당 시간이 얼마나 걸리는지 계산
	CKeyMgr::getInstance()->tick();
	CLevelMgr::getInstance()->tick();
	//TempTick();			// 매 프레임마다 호출 (Object Tick)


	// 화면 클리어 (Target Clear)
	//float clear_color[4] = { 0.3f, 0.3f, 0.3f, 1.f };
	//CDevice::getInstance()->clearTarget(clear_color);			// 모두 지우고

	// 그린다 (Object Render)
	// Level->render();							// 렌더타겟에 그린다
	//TempRender();
	//CLevelMgr::getInstance()->render();

	// 게시한다 (Present)
	// SwapChain->Present();				// 윈도우 화면에 보낸다
	//CDevice::getInstance()->present();

	CRenderMgr::getInstance()->render();
}

CEngine::CEngine()
	: m_hWnd(nullptr)
	, m_resolution{}
{

}

CEngine::~CEngine()
{
	//TempRelease();
}
