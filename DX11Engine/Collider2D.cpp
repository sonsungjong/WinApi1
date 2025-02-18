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
	// ���� ��ġ�� ���
	Vec3 vOjbPos = Transform()->getRelativePos();				// ��ġ�� �����ͼ�
	m_finalPos = vOjbPos + m_offset;								// offset�� ���Ѵ�

	if (false == m_bAbsolute)
	{
		// ��ü�� ũ�⿡ ������ �ް� (ũ�� -> ���� -> �̵� : ũ���� ������)
		Matrix matScale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
		Matrix matRotation = XMMatrixRotationZ(m_rotation.z);
		Matrix matTrans = XMMatrixTranslation(m_offset.x, m_offset.y, m_offset.z);

		m_matColWorld = matScale * matRotation * matTrans;
		m_matColWorld *= Transform()->getWorldMatrix();
	}
	else
	{
		// ��ü�� ũ�⿡ ������ �ȹް�

	}
}
