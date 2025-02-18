#include "pch.h"
#include "Collider2D.h"

#include "Transform.h"

CCollider2D::CCollider2D()
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_scale(Vec3(1.f, 1.f, 1.f))
	, m_bAbsolute(false)
{
}

CCollider2D::~CCollider2D()
{

}

void CCollider2D::finaltick()
{
	// 최종 위치를 계산
	Vec3 vOjbPos = Transform()->getRelativePos();				// 위치를 가져와서
	m_finalPos = vOjbPos + m_offset;								// offset을 더한다

	if (false == m_bAbsolute)
	{
		// 물체의 크기에 영향을 받게 (크기 -> 자전 -> 이동 : 크자이 순서로)
		Matrix matScale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		Matrix matRotation = XMMatrixRotationZ(m_rotation.z);
		Matrix matTrans = XMMatrixTranslation(m_offset.x, m_offset.y, m_offset.z);

		m_matColWorld = matScale * matRotation * matTrans;
		m_matColWorld *= Transform()->getWorldMatrix();
	}
	else
	{
		// 물체의 크기에 영향을 안받게

	}
}
