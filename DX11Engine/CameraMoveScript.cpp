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
	// ī�޶� ���� ��� ����
	if (KEY_TAP(KEY::P))
	{
		Camera()->getProjType() == PROJ_TYPE::ORTHOGRAPHIC ? Camera()->setProjType(PROJ_TYPE::PERSPECTIVE) : Camera()->setProjType(PROJ_TYPE::ORTHOGRAPHIC);
		if (Camera()->getProjType() == PROJ_TYPE::ORTHOGRAPHIC)
		{
			// ���� ����������忴�ٰ� ������������ ����� (ȸ�� ���¸� �ʱ�ȭ�ؼ� ������ �ٶ󺸰��Ѵ��� Z���� 0�������� ���� ������������ �����Ų��)
			Vec3 vPos = Transform()->getRelativePos();
			Transform()->setRelativePos(Vec3(vPos.x, vPos.y, 0.f));				// z���� 0���� �ʱ�ȭ
			Transform()->setRelativeRotation(Vec3(0.f, 0.f, 0.f));					// ȸ������ 0���� �ʱ�ȭ (����)
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

// ���� ���� �� �̵�ó��
void CCameraMoveScript::MoveByPerspective()
{
	// Shift �ӵ� ����
	float speed = m_speed;
	if (KEY_PRESSED(KEY::LSHIFT))
	{
		speed *= 4.f;
	}

	// Ű �Է¿� ���� ��ġ�̵�
	Vec3 vCurPos = Transform()->getRelativePos();

	// ���⺤��
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

	// ������ Ŭ���� ȸ��
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vDrag = CKeyMgr::getInstance()->getMouseDrag();

		Vec3 vRotation = Transform()->getRelativeRotation();
		vRotation.y += vDrag.x * DT * XM_PI * 20.f;
		vRotation.x -= vDrag.y * DT * XM_PI * 20.f;

		Transform()->setRelativeRotation(vRotation);
	}

	// ��ġ ����
	getOwner()->Transform()->setRelativePos(vCurPos);
}

// ���� ���� �� �̵�ó��
void CCameraMoveScript::MoveByOrthographic()
{
	// Shift �ӵ� ����
	float speed = m_speed;
	if (KEY_PRESSED(KEY::LSHIFT))
	{
		speed *= 4.f;
	}

	// Ű �Է¿� ���� ��ġ�̵�
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
		
		// 100�� �̻� ������ ���ϰ�
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

		// 10�� �̻� �ܾƿ��� ���ϰ� (1�� : ����ũ��)
		if (scale > 10.0f)
		{
			scale = 10.0f;
		}
		Camera()->setScale(scale);
	}
	if (KEY_PRESSED(KEY::F4))
	{
		// 1�� : ����ũ��
		float scale = 1.f;

		Camera()->setScale(scale);
	}

	// ��ġ ����
	getOwner()->Transform()->setRelativePos(vCurPos);
}
