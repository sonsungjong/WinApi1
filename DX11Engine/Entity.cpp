#include "pch.h"
#include "Entity.h"

ULONGLONG CEntity::g_NextID = 0U;

CEntity::CEntity()
	: m_id(g_NextID++)
{
	// ���������� ���� ���̵� 1�� �ø��鼭 ���̵� �ο�
}

CEntity::CEntity(const CEntity& _origin)
	: m_id(g_NextID++)
{
	// ��������ڸ� ���� �����ؼ� Ŭ���� �����ÿ� ���̵��� �������� �ʰ� �Ѵ�
}

CEntity::~CEntity()
{
}
