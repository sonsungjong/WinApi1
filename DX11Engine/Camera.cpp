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
	// View 행렬 계산
	// 1. 먼저 이동을 시킨다
	Vec3 vCamWorldPos = getOwner()->getTransform()->getRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamWorldPos.x, -vCamWorldPos.y, -vCamWorldPos.z);			// 카메라 위치의 반대값으로 이동행렬 셋팅 (카메라를 원점으로 배치시키는 만큼 물체한테도 적용)
	
	// 2. 그 다음 회전을 시킨다
	// 카메라가 바라보는 방향을 원점의 z축으로 회전시키는 만큼 물체에도 적용시킨다 => (0,0,1)
	Vec3 vR = getTransform()->getRelativeDirection(DIR_TYPE::RIGHT);
	Vec3 vU = getTransform()->getRelativeDirection(DIR_TYPE::UP);
	Vec3 vF = getTransform()->getRelativeDirection(DIR_TYPE::FRONT);

	Matrix matViewRotation = XMMatrixIdentity();
	matViewRotation._11 = vR.x;    matViewRotation._12 = vU.x;    matViewRotation._13 = vF.x;
	matViewRotation._21 = vR.y;    matViewRotation._22 = vU.y;    matViewRotation._23 = vF.y;
	matViewRotation._31 = vR.z;    matViewRotation._32 = vU.z;    matViewRotation._33 = vF.z;

	m_matView = matViewTrans * matViewRotation;				// 행렬은 곱하는 순서가 중요하다 ([공전] : 이동 * 회전, [자전] : 회전 * 이동)

	// Proj 행렬 계산
	Vec2 vRenderResolution = CDevice::getInstance()->getRenderResolution();

	// 해상도 비율
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

