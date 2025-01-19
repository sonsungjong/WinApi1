#include "pch.h"
#include "RenderMgr.h"

#include "Device.h"
#include "Camera.h"

CRenderMgr::CRenderMgr()
{

}

CRenderMgr::~CRenderMgr()
{

}

void CRenderMgr::init()
{

}

void CRenderMgr::tick()
{
}

void CRenderMgr::render()
{
	// 화면 클리어 (Target Clear)
	float clear_color[4] = { 0.3f, 0.3f, 0.3f, 1.f };
	CDevice::getInstance()->clearTarget(clear_color);			// 모두 지우고

	// 카메라가 보고있는 장면을 렌더타겟에 그린다 (Object Render)
	for (size_t i = 0U; i < m_vecCam.size(); ++i) {
		m_vecCam[i]->render();
	}

	// 게시한다 (Present)
	CDevice::getInstance()->present();
}

void CRenderMgr::registerCamera(CCamera* _Cam, int _priority)
{
	for (size_t i = 0U; i < m_vecCam.size(); ++i)
	{
		if (m_vecCam[i] == _Cam)
		{
			if (_priority == i)
			{
				return;						// 같은 우선순위에 같은 카메라 주소가 있으면 바로 리턴시킨다
			}
			else {
				m_vecCam[i] = nullptr;					// 다른 우선순위에 추가하려한다면 기존에 것은 없앤다
			}
		}
	}

	// 예외처리 : 벡터 사이즈 증가
	if (m_vecCam.size() <= _priority) 
	{
		m_vecCam.resize(_priority + 1);
	}

	// 해당 우선순위에 카메라 주소를 등록
	m_vecCam[_priority] = _Cam;
}
