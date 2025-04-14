#include "pch.h"
#include "PlayerScript.h"


CPlayerScript::CPlayerScript()
	: m_speed(100.f)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::tick()
{
	// 키 입력에 따른 플레이어의 위치이동
	Vec3 vCurPos = getOwner()->Transform()->getRelativePos();

	if (KEY_PRESSED(KEY::UP))
	{
		vCurPos.y += DT * m_speed;
	}
	if (KEY_PRESSED(KEY::DOWN))
	{
		vCurPos.y -= DT * m_speed;
	}
	if (KEY_PRESSED(KEY::LEFT))
	{
		vCurPos.x -= DT * m_speed;
	}
	if (KEY_PRESSED(KEY::RIGHT))
	{
		vCurPos.x += DT * m_speed;
	}

	if (KEY_PRESSED(KEY::Z))
	{
		// 회전
		Vec3 vRotation = getOwner()->Transform()->getRelativeRotation();
		vRotation.z += DT * XM_PI;
		getOwner()->Transform()->setRelativeRotation(vRotation);

		// z축으로 밀기
		//vCurPos.z += DT * m_speed;
	}

	getOwner()->Transform()->setRelativePos(vCurPos);
}