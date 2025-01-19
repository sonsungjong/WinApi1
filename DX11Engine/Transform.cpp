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
	Matrix matScale = XMMatrixIdentity();
	m_matWorld._11 = m_RelativeScale.x;
	m_matWorld._22 = m_RelativeScale.y;
	m_matWorld._33 = m_RelativeScale.z;

	// Z축 회전 행렬
	Matrix matZRotation = XMMatrixIdentity();
	matZRotation._11 = cosf(m_RelativeRotation.z);
	matZRotation._12 = sinf(m_RelativeRotation.z);
	matZRotation._21 = -sinf(m_RelativeRotation.z);
	matZRotation._22 = cosf(m_RelativeRotation.z);

	// 이동 행렬
	Matrix matTranslation = XMMatrixIdentity();
	matTranslation._41 = m_RelativePos.x;
	matTranslation._42 = m_RelativePos.y;
	matTranslation._43 = m_RelativePos.z;

	m_matWorld = matScale * matZRotation * matTranslation;				// 스케일 x 회전 x 이동
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