#include "pch.h"
#include "Transform.h"

#include "Device.h"
#include "ConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_RelativeScale(Vec3(1.0f, 1.0f, 1.0f))				// ��ü ũ�� ���� (x 1����, y 1����, z 1����)
{
}

CTransform::~CTransform()
{
}

void CTransform::finaltick()
{
	m_matWorld = XMMatrixIdentity();				// ������ķ� �ʱ�ȭ

	m_matWorld._11 = m_RelativeScale.x;
	m_matWorld._22 = m_RelativeScale.y;
	m_matWorld._33 = m_RelativeScale.z;

	m_matWorld._41 = m_RelativePos.x;
	m_matWorld._42 = m_RelativePos.y;
	m_matWorld._43 = m_RelativePos.z;	
}

// ����ִ� �����͸� GPU�� ������
void CTransform::binding()
{
	// SysMem -> GPU
	CConstBuffer* pCB = CDevice::getInstance()->getConstBuffer(CB_TYPE::TRANSFORM);

	tTransform trans = {};
	trans.matWorld = m_matWorld;

	pCB->setData(&trans);
	pCB->binding();				// b0 ��� �������Ϳ� ����
}