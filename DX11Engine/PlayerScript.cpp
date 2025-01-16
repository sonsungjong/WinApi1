#include "pch.h"
#include "PlayerScript.h"


CPlayerScript::CPlayerScript()
	: m_speed(1.f)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::tick()
{
	// Ű �Է¿� ���� ��ġ�̵�
	Vec3 vCurPos = getOwner()->getTransform()->getRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vCurPos.y += DT * m_speed;
	}
	if (KEY_PRESSED(KEY::S))
	{
		vCurPos.y -= DT * m_speed;
	}
	if (KEY_PRESSED(KEY::A))
	{
		vCurPos.x -= DT * m_speed;
	}
	if (KEY_PRESSED(KEY::D))
	{
		vCurPos.x += DT * m_speed;
	}

	getOwner()->getTransform()->setRelativePos(vCurPos);
}