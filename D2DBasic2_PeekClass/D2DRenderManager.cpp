#include "pch.h"
#include "D2DRenderManager.h"

D2DRenderManager& D2DRenderManager::getInstance()
{
	static D2DRenderManager instance;
	return instance;
}

void D2DRenderManager::init()
{
	if (!m_factory)
	{
		(void)D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory);
	}
}

void D2DRenderManager::destroy()
{
	for (auto& [hWnd, info] : m_umapTargets)
	{
		if (info.pCopyBitmap) {
			info.pCopyBitmap->Release();
		}
		if (info.pBackBuffer) {
			info.pBackBuffer->Release();
		}
		if (info.pRenderTarget) {
			info.pRenderTarget->Release();
		}
	}
	m_umapTargets.clear();

	if (m_factory)
	{
		m_factory->Release();
		m_factory = NULL;
	}
}

void D2DRenderManager::addTargetWnd(HWND hWnd, std::function<void(ID2D1BitmapRenderTarget*)> drawFunc)
{
	RECT rc; 
	GetClientRect(hWnd, &rc);
	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	ST_RenderTargetInfo info = {};
	info.hWnd = hWnd;
	info.drawCallback = drawFunc;

	if (m_factory)
	{
		HRESULT hr = m_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, size), &info.pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = info.pRenderTarget->CreateCompatibleRenderTarget(&info.pBackBuffer);
		}

		if (m_umapTargets.find(hWnd) == m_umapTargets.end())
		{
			m_umapTargets[hWnd] = info;
		}
	}
}

void D2DRenderManager::removeTargetWnd(HWND hWnd)
{
	auto it = m_umapTargets.find(hWnd);
	if (it != m_umapTargets.end())
	{
		ST_RenderTargetInfo& info = it->second;
		if (info.pCopyBitmap) {
			info.pCopyBitmap->Release();
		}
		if (info.pBackBuffer) {
			info.pBackBuffer->Release();
		}
		if (info.pRenderTarget) {
			info.pRenderTarget->Release();
		}
		m_umapTargets.erase(it);
	}
}

ID2D1BitmapRenderTarget* D2DRenderManager::getBackBuffer(HWND hWnd)
{
	ID2D1BitmapRenderTarget* p_target = NULL;
	auto it = m_umapTargets.find(hWnd);
	if (it != m_umapTargets.end())
	{
		p_target = it->second.pBackBuffer;
	}

	return p_target;
}

void D2DRenderManager::updateView(HWND hWnd)
{
	auto it = m_umapTargets.find(hWnd);
	if (it != m_umapTargets.end())
	{
		ST_RenderTargetInfo& info = it->second;

		if (info.drawCallback && info.pBackBuffer)
		{
			info.drawCallback(info.pBackBuffer);
		}

		if (info.pCopyBitmap)
		{
			info.pCopyBitmap->Release();
			info.pCopyBitmap = NULL;
		}

		if (info.pBackBuffer)
		{
			info.pBackBuffer->GetBitmap(&info.pCopyBitmap);
		}

		if (info.pRenderTarget && info.pCopyBitmap)
		{
			info.pRenderTarget->BeginDraw();
			info.pRenderTarget->Clear();
			info.pRenderTarget->DrawBitmap(info.pCopyBitmap);
			info.pRenderTarget->EndDraw();

			info.pCopyBitmap->Release();
			info.pCopyBitmap = NULL;
		}
	}
}

void D2DRenderManager::updateViewAll()
{
	for (auto& [hWnd, _] : m_umapTargets)
	{
		updateView(hWnd);
	}
}

bool D2DRenderManager::isEmptyMap() const
{
	return m_umapTargets.empty();
}