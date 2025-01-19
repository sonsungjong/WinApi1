#include "pch.h"
#include "Camera.h"

#include "LevelMgr.h"
#include "Level.h"
#include "Layer.h"
#include "GameObject.h"
#include "RenderComponent.h"

#include "RenderMgr.h"
#include "Transform.h"
#include "Device.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_nCamPriority(-1)
	, m_far(1000.f)
{

}

CCamera::~CCamera()
{
}

void CCamera::finaltick()
{
	// View ��� ���
	Vec3 vCamWorldPos = getOwner()->getTransform()->getRelativePos();

	m_matView = XMMatrixTranslation(-vCamWorldPos.x, -vCamWorldPos.y, -vCamWorldPos.z);			// ī�޶� �̵��� �ݴ밪���� ����

	// Proj ��� ���
	Vec2 vRenderResolution = CDevice::getInstance()->getRenderResolution();

	// �ػ� ����
	float AspectRatio = vRenderResolution.x / vRenderResolution.y;

	m_matProj = XMMatrixPerspectiveFovLH((XM_PI / 3.f), AspectRatio, 1.f, m_far);
}

void CCamera::render()
{
	g_Trans.matView = m_matView;
	g_Trans.matProj = m_matProj;

	CLevel* pCurLevel = CLevelMgr::getInstance()->getCurrentLevel();

	for (UINT i = 0U; i < MAX_LAYER; ++i) {
		CLayer* pLayer = pCurLevel->getLayer(i);
		pLayer->render();
	}
}

void CCamera::setCameraPriority(int _priority)
{
	m_nCamPriority = _priority;

	if (m_nCamPriority >= 0) 
	{
		CRenderMgr::getInstance()->registerCamera(this, m_nCamPriority);
	}
}

