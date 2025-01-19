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

	// ������ ���
	Matrix matScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z);

	// Z�� ȸ�� ���
	Matrix matRotation = XMMatrixRotationX(m_RelativeRotation.x);
	matRotation *= XMMatrixRotationY(m_RelativeRotation.y);
	matRotation *= XMMatrixRotationZ(m_RelativeRotation.z);

	// �̵� ���
	Matrix matTranslation = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);

	m_matWorld = matScale * matRotation * matTranslation;				// ������ x ȸ�� x �̵�

	// ������Ʈ�� �������� ���
	m_RelativeDirection[(UINT)DIR_TYPE::RIGHT] = XAxis;
	m_RelativeDirection[(UINT)DIR_TYPE::UP] = YAxis;
	m_RelativeDirection[(UINT)DIR_TYPE::FRONT] = ZAxis;

	m_RelativeDirection[(UINT)DIR_TYPE::RIGHT]  = XMVector3TransformNormal(m_RelativeDirection[(UINT)DIR_TYPE::RIGHT], matRotation);
	m_RelativeDirection[(UINT)DIR_TYPE::RIGHT].Normalize();
	m_RelativeDirection[(UINT)DIR_TYPE::UP]  = XMVector3TransformNormal(m_RelativeDirection[(UINT)DIR_TYPE::UP], matRotation);
	m_RelativeDirection[(UINT)DIR_TYPE::UP].Normalize();
	m_RelativeDirection[(UINT)DIR_TYPE::FRONT]  = XMVector3TransformNormal(m_RelativeDirection[(UINT)DIR_TYPE::FRONT], matRotation);
	m_RelativeDirection[(UINT)DIR_TYPE::FRONT].Normalize();
	
}

// ����ִ� �����͸� GPU�� ������
void CTransform::binding()
{
	// SysMem -> GPU
	CConstBuffer* pCB = CDevice::getInstance()->getConstBuffer(CB_TYPE::TRANSFORM);

	//g_Trans.matView;			// ī�޶��� �� ���
	g_Trans.matWorld = m_matWorld;

	pCB->setData(&g_Trans);
	pCB->binding();				// b0 ��� �������Ϳ� ����
}