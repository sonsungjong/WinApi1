#include "pch.h"
#include "Transform.h"

#include "Device.h"
#include "ConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_RelativeScale(Vec3(1.f, 1.f, 1.f))				// 물체 크기 배율 (x 1배율, y 1배율, z 1배율)
{
}

CTransform::~CTransform()
{
}

void CTransform::finaltick()
{
	
}

// 들고있는 데이터를 GPU로 보낸다
void CTransform::binding()
{
	// SysMem -> GPU
	CConstBuffer* pCB = CDevice::getInstance()->getConstBuffer(CB_TYPE::TRANSFORM);

	tTransform trans = {};
	trans.Position = m_RelativePos;
	trans.Scale = m_RelativeScale;

	pCB->setData(&trans);
	pCB->binding();				// b0 상수 레지스터에 전달
}