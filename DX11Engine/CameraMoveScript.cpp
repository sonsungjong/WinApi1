#include "pch.h"
#include "CameraMoveScript.h"

CCameraMoveScript::CCameraMoveScript()
	: m_speed(200.f)
{
}

CCameraMoveScript::~CCameraMoveScript()
{
}

void CCameraMoveScript::tick()
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
