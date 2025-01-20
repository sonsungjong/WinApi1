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
	// Shift 속도 배율
	float speed = m_speed;
	if (KEY_PRESSED(KEY::LSHIFT))
	{
		speed *= 4.f;
	}

	// 키 입력에 따른 위치이동
	Vec3 vCurPos = getTransform()->getRelativePos();

	Vec3 vFront = getTransform()->getRelativeDirection(DIR_TYPE::FRONT);
	Vec3 vRight = getTransform()->getRelativeDirection(DIR_TYPE::RIGHT);

	if (KEY_PRESSED(KEY::W))
	{
		vCurPos += vFront * speed * DT;
	}
	if (KEY_PRESSED(KEY::S))
	{
		vCurPos += -vFront * speed * DT;
	}
	if (KEY_PRESSED(KEY::A))
	{
		vCurPos += -vRight * speed * DT;
	}
	if (KEY_PRESSED(KEY::D))
	{
		vCurPos += vRight * speed * DT;
	}

	if (KEY_PRESSED(KEY::RBTN)) 
	{
		Vec2 vDrag = CKeyMgr::getInstance()->getMouseDrag();

		Vec3 vRotation = getTransform()->getRelativeRotation();
		vRotation.y += vDrag.x * DT * XM_PI * 20.f;
		vRotation.x -= vDrag.y * DT * XM_PI * 20.f;

		getTransform()->setRelativeRotation(vRotation);
	}

	getOwner()->getTransform()->setRelativePos(vCurPos);
}
