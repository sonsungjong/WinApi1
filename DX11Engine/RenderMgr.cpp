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
	// ȭ�� Ŭ���� (Target Clear)
	float clear_color[4] = { 0.3f, 0.3f, 0.3f, 1.f };
	CDevice::getInstance()->clearTarget(clear_color);			// ��� �����

	// ī�޶� �����ִ� ����� ����Ÿ�ٿ� �׸��� (Object Render)
	for (size_t i = 0U; i < m_vecCam.size(); ++i) {
		m_vecCam[i]->render();
	}

	// �Խ��Ѵ� (Present)
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
				return;						// ���� �켱������ ���� ī�޶� �ּҰ� ������ �ٷ� ���Ͻ�Ų��
			}
			else {
				m_vecCam[i] = nullptr;					// �ٸ� �켱������ �߰��Ϸ��Ѵٸ� ������ ���� ���ش�
			}
		}
	}

	// ����ó�� : ���� ������ ����
	if (m_vecCam.size() <= _priority) 
	{
		m_vecCam.resize(_priority + 1);
	}

	// �ش� �켱������ ī�޶� �ּҸ� ���
	m_vecCam[_priority] = _Cam;
}
