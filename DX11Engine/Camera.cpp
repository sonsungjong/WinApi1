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
	// 1. ���� �̵��� ��Ų��
	Vec3 vCamWorldPos = getOwner()->getTransform()->getRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamWorldPos.x, -vCamWorldPos.y, -vCamWorldPos.z);			// ī�޶� ��ġ�� �ݴ밪���� �̵���� ���� (ī�޶� �������� ��ġ��Ű�� ��ŭ ��ü���׵� ����)
	
	// 2. �� ���� ȸ���� ��Ų��
	// ī�޶� �ٶ󺸴� ������ ������ z������ ȸ����Ű�� ��ŭ ��ü���� �����Ų�� => (0,0,1)
	Vec3 vR = getTransform()->getRelativeDirection(DIR_TYPE::RIGHT);
	Vec3 vU = getTransform()->getRelativeDirection(DIR_TYPE::UP);
	Vec3 vF = getTransform()->getRelativeDirection(DIR_TYPE::FRONT);

	Matrix matViewRotation = XMMatrixIdentity();
	matViewRotation._11 = vR.x;    matViewRotation._12 = vU.x;    matViewRotation._13 = vF.x;
	matViewRotation._21 = vR.y;    matViewRotation._22 = vU.y;    matViewRotation._23 = vF.y;
	matViewRotation._31 = vR.z;    matViewRotation._32 = vU.z;    matViewRotation._33 = vF.z;

	m_matView = matViewTrans * matViewRotation;				// ����� ���ϴ� ������ �߿��ϴ� ([����] : �̵� * ȸ��, [����] : ȸ�� * �̵�)

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

