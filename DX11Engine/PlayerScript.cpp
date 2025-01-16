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
	// 키 입력에 따른 위치이동
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