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
	, m_ProjType(PROJ_TYPE::PERSPECTIVE)
	, m_nCamPriority(-1)
	, m_FOV((XM_PI / 3.f))
	, m_far(10000.f)
	, m_width(0.f)
	, m_scale(1.f)				// 직교투영 배율값
{
	Vec2 vRenderResolution = CDevice::getInstance()->getRenderResolution();
	m_width = vRenderResolution.x;
	m_aspectRatio = vRenderResolution.x / vRenderResolution.y;
}

CCamera::~CCamera()
{
}

void CCamera::finaltick()
{
	// View 행렬 계산
	// 1. 먼저 이동을 시킨다
	Vec3 vCamWorldPos = getOwner()->Transform()->getRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamWorldPos.x, -vCamWorldPos.y, -vCamWorldPos.z);			// 카메라 위치의 반대값으로 이동행렬 셋팅 (카메라를 원점으로 배치시키는 만큼 물체한테도 적용)
	
	// 2. 그 다음 회전을 시킨다
	// 카메라가 바라보는 방향을 원점의 z축으로 회전시키는 만큼 물체에도 적용시킨다 => (0,0,1)
	Vec3 vR = Transform()->getRelativeDirection(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->getRelativeDirection(DIR_TYPE::UP);
	Vec3 vF = Transform()->getRelativeDirection(DIR_TYPE::FRONT);

	Matrix matViewRotation = XMMatrixIdentity();
	matViewRotation._11 = vR.x;    matViewRotation._12 = vU.x;    matViewRotation._13 = vF.x;
	matViewRotation._21 = vR.y;    matViewRotation._22 = vU.y;    matViewRotation._23 = vF.y;
	matViewRotation._31 = vR.z;    matViewRotation._32 = vU.z;    matViewRotation._33 = vF.z;

	m_matView = matViewTrans * matViewRotation;				// 행렬은 곱하는 순서가 중요하다 ([공전] : 이동 * 회전, [자전] : 회전 * 이동)

	// Proj 행렬 계산
	if (PROJ_TYPE::PERSPECTIVE == m_ProjType)
	{
		// 원근 투영일때는 시야각 반영
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_aspectRatio, 1.f, m_far);
	}
	else
	{
		// 직교 투영일때는 시야각이 없음
		m_matProj = XMMatrixOrthographicLH(m_width * m_scale, (m_width / m_aspectRatio) * m_scale, 1.f, m_far);
	}
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

