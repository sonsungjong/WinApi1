#include "pch.h"
#include "Transform.h"

#include "Device.h"
#include "ConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_RelativeScale(Vec3(1.f, 1.f, 1.f))				// ��ü ũ�� ���� (x 1����, y 1����, z 1����)
{
}

CTransform::~CTransform()
{
}

void CTransform::finaltick()
{
	
}

// ����ִ� �����͸� GPU�� ������
void CTransform::binding()
{
	// SysMem -> GPU
	CConstBuffer* pCB = CDevice::getInstance()->getConstBuffer(CB_TYPE::TRANSFORM);

	tTransform trans = {};
	trans.Position = m_RelativePos;
	trans.Scale = m_RelativeScale;

	pCB->setData(&trans);
	pCB->binding();				// b0 ��� �������Ϳ� ����
}