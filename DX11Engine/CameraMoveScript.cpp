#include "pch.h"
#include "CameraMoveScript.h"
#include "Camera.h"

CCameraMoveScript::CCameraMoveScript()
	: m_speed(200.f)
{
}

CCameraMoveScript::~CCameraMoveScript()
{
}

void CCameraMoveScript::tick()
{
	// 카메라 투영 모드 변경
	if (KEY_TAP(KEY::P))
	{
		Camera()->getProjType() == PROJ_TYPE::ORTHOGRAPHIC ? Camera()->setProjType(PROJ_TYPE::PERSPECTIVE) : Camera()->setProjType(PROJ_TYPE::ORTHOGRAPHIC);
		if (Camera()->getProjType() == PROJ_TYPE::ORTHOGRAPHIC)
		{
			// 원래 원근투영모드였다가 직교투영모드로 변경됨 (회전 상태를 초기화해서 정면을 바라보게한다음 Z축을 0라인으로 당기고 직교투영모드로 변경시킨다)
			Vec3 vPos = Transform()->getRelativePos();
			Transform()->setRelativePos(Vec3(vPos.x, vPos.y, 0.f));				// z축을 0으로 초기화
			Transform()->setRelativeRotation(Vec3(0.f, 0.f, 0.f));					// 회전량을 0으로 초기화 (정면)
		}
	}

	if (Camera()->getProjType() == PROJ_TYPE::PERSPECTIVE)
	{
		MoveByPerspective();
	}
	else
	{
		MoveByOrthographic();
	}
}

// 원근 투영 시 이동처리
void CCameraMoveScript::MoveByPerspective()
{
	// Shift 속도 배율
	float speed = m_speed;
	if (KEY_PRESSED(KEY::LSHIFT))
	{
		speed *= 4.f;
	}

	// 키 입력에 따른 위치이동
	Vec3 vCurPos = Transform()->getRelativePos();

	// 방향벡터
	Vec3 vFront = Transform()->getRelativeDirection(DIR_TYPE::FRONT);
	Vec3 vRight = Transform()->getRelativeDirection(DIR_TYPE::RIGHT);

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

	// 오른쪽 클릭시 회전
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vDrag = CKeyMgr::getInstance()->getMouseDrag();

		Vec3 vRotation = Transform()->getRelativeRotation();
		vRotation.y += vDrag.x * DT * XM_PI * 20.f;
		vRotation.x -= vDrag.y * DT * XM_PI * 20.f;

		Transform()->setRelativeRotation(vRotation);
	}

	// 위치 적용
	getOwner()->Transform()->setRelativePos(vCurPos);
}

// 직교 투영 시 이동처리
void CCameraMoveScript::MoveByOrthographic()
{
	// Shift 속도 배율
	float speed = m_speed;
	if (KEY_PRESSED(KEY::LSHIFT))
	{
		speed *= 4.f;
	}

	// 키 입력에 따른 위치이동
	Vec3 vCurPos = Transform()->getRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vCurPos.y += speed * DT;
	}
	if (KEY_PRESSED(KEY::S))
	{
		vCurPos.y -= speed * DT;
	}
	if (KEY_PRESSED(KEY::A))
	{
		vCurPos.x -= speed * DT;
	}
	if (KEY_PRESSED(KEY::D))
	{
		vCurPos.x += speed * DT;
	}
	if (KEY_PRESSED(KEY::F2))
	{
		float scale = Camera()->getScale();
		scale -= DT;
		
		// 100배 이상 줌인을 못하게
		if (scale < 0.01f) 
		{
			scale = 0.01f;
		}
		Camera()->setScale(scale);
	}
	if (KEY_PRESSED(KEY::F3))
	{
		float scale = Camera()->getScale();
		scale += DT;

		// 10배 이상 줌아웃을 못하게 (1배 : 원본크기)
		if (scale > 10.0f)
		{
			scale = 10.0f;
		}
		Camera()->setScale(scale);
	}
	if (KEY_PRESSED(KEY::F4))
	{
		// 1배 : 원본크기
		float scale = 1.f;

		Camera()->setScale(scale);
	}

	// 위치 적용
	getOwner()->Transform()->setRelativePos(vCurPos);
}
