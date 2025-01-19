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
	// 키 입력에 따른 위치이동
	Vec3 vCurPos = getTransform()->getRelativePos();

	Vec3 vFront = getTransform()->getRelativeDirection(DIR_TYPE::FRONT);
	Vec3 vRight = getTransform()->getRelativeDirection(DIR_TYPE::RIGHT);

	if (KEY_PRESSED(KEY::W))
	{
		vCurPos += vFront * m_speed * DT;
	}
	if (KEY_PRESSED(KEY::S))
	{
		vCurPos += -vFront * m_speed * DT;
	}
	if (KEY_PRESSED(KEY::A))
	{
		vCurPos += -vRight * m_speed * DT;
	}
	if (KEY_PRESSED(KEY::D))
	{
		vCurPos += vRight * m_speed * DT;
	}

	if (KEY_PRESSED(KEY::Y))
	{
		Vec3 vRotation = getTransform()->getRelativeRotation();
		vRotation.y += DT * XM_PI;
		getTransform()->setRelativeRotation(vRotation);
	}

	getOwner()->getTransform()->setRelativePos(vCurPos);
}
