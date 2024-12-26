#include "pch.h"
#include "Entity.h"

ULONGLONG CEntity::g_NextID = 0U;

CEntity::CEntity()
	: m_id(g_NextID++)
{
	// 정적변수를 통해 아이디를 1씩 늘리면서 아이디 부여
}

CEntity::CEntity(const CEntity& _origin)
	: m_id(g_NextID++)
{
	// 복사생성자를 직접 구현해서 클래스 복제시에 아이디값이 복제되지 않게 한다
}

CEntity::~CEntity()
{
}
