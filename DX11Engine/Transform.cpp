#include "pch.h"
#include "Transform.h"

#include "Device.h"
#include "ConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_RelativeScale(Vec3(1.0f, 1.0f, 1.0f))				// 물체 크기 배율 (x 1배율, y 1배율, z 1배율)
{
}

CTransform::~CTransform()
{
}

void CTransform::finaltick()
{
	m_matWorld = XMMatrixIdentity();				// 단위행렬로 초기화

	// 스케일 행렬
	Matrix matScale = XMMatrixScaling(m_RelativeScale.x, m_RelativeScale.y, m_RelativeScale.z);

	// Z축 회전 행렬
	Matrix matRotation = XMMatrixRotationX(m_RelativeRotation.x);
	matRotation *= XMMatrixRotationY(m_RelativeRotation.y);
	matRotation *= XMMatrixRotationZ(m_RelativeRotation.z);

	// 이동 행렬
	Matrix matTranslation = XMMatrixTranslation(m_RelativePos.x, m_RelativePos.y, m_RelativePos.z);

	m_matWorld = matScale * matRotation * matTranslation;				// 스케일 x 회전 x 이동
}

// 들고있는 데이터를 GPU로 보낸다
void CTransform::binding()
{
	// SysMem -> GPU
	CConstBuffer* pCB = CDevice::getInstance()->getConstBuffer(CB_TYPE::TRANSFORM);

	tTransform trans = {};
	trans.matWorld = m_matWorld;

	pCB->setData(&trans);
	pCB->binding();				// b0 상수 레지스터에 전달
}