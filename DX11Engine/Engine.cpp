#include "pch.h"
#include "Engine.h"

#include "Device.h"
#include "TimeMgr.h"
#include "PathMgr.h"
#include "KeyMgr.h"
#include "AssetMgr.h"
#include "LevelMgr.h"
#include "RenderMgr.h"


// ���� ����� ������ �ʱ�ȭ�� ������Ѵ�
//CEngine* CEngine::g_this = nullptr;

// ������� �ػ�
int CEngine::init(HWND _hWnd, Vec2 _resolution)
{
	m_hWnd = _hWnd;							// ���������
	m_resolution = _resolution;				// �ػ�

	::RECT rt = { 0, 0, static_cast<LONG>(m_resolution.x), static_cast<LONG>(m_resolution.y) };

	// ������ ũ�� ����
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);								// �޴��� false
	SetWindowPos(m_hWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);

	// DirectX11
	if (!SUCCEEDED(CDevice::getInstance()->init(m_hWnd, m_resolution))) 
	{
		::MessageBox(m_hWnd, L"Device �ʱ�ȭ ����", L"���� �ʱ�ȭ ����", MB_OK);
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
	//	MessageBox(m_hWnd, L"Device �ʱ�ȭ ����", L"Temp Init �ʱ�ȭ ����", MB_OK);
	//	return E_FAIL;
	//}

	return S_OK;
}

void CEngine::progress()
{
	// Level->tick();								// ��ƽ�� ������ �ൿ
	// Manager Tick
	CTimeMgr::getInstance()->tick();				// �������Ӵ� �ð��� �󸶳� �ɸ����� ���
	CKeyMgr::getInstance()->tick();
	CLevelMgr::getInstance()->tick();
	//TempTick();			// �� �����Ӹ��� ȣ�� (Object Tick)


	// ȭ�� Ŭ���� (Target Clear)
	//float clear_color[4] = { 0.3f, 0.3f, 0.3f, 1.f };
	//CDevice::getInstance()->clearTarget(clear_color);			// ��� �����

	// �׸��� (Object Render)
	// Level->render();							// ����Ÿ�ٿ� �׸���
	//TempRender();
	//CLevelMgr::getInstance()->render();

	// �Խ��Ѵ� (Present)
	// SwapChain->Present();				// ������ ȭ�鿡 ������
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
