#include "pch.h"
#include "KeyMgr.h"

#include "Engine.h"

// enum�� ���� Ű���� �ƽ�Ű���� ��Ī������ ���� �迭
UINT g_KeyValue[(UINT)KEY::KEY_END] =
{
	'W',
	'S',
	'A',
	'D',
	'Z',
	'X',
	'C',
	'V',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,
	VK_LBUTTON,
	VK_RBUTTON,
	VK_RETURN,			// ���� Ű
	VK_ESCAPE,			// ECS Ű
	VK_SPACE,
	VK_LSHIFT,
	VK_MENU,				// ALT Ű
	VK_CONTROL,		// CTRL Ű
};

CKeyMgr::CKeyMgr()
{

}

CKeyMgr::~CKeyMgr()
{

}

void CKeyMgr::init()
{
	for (UINT i = 0U; i < (UINT)KEY::KEY_END; ++i) 
	{
		STKeyInfo info = {};
		info.State = KEY_STATE::NONE;
		info.PrevPressed = false;

		m_vecKey.push_back(info);
	}
}

void CKeyMgr::tick()
{
	for (size_t i = 0U; i < m_vecKey.size(); ++i) 
	{
		if (GetAsyncKeyState(g_KeyValue[i]) & 0x8001)
		{
			// KEY enum���� �ش��ϴ� ���� ������
			if (m_vecKey[i].PrevPressed == false)
			{
				// ���� frame���� �ȴ��� �־���
				m_vecKey[i].State = KEY_STATE::TAP;				// ó�� ����
			}
			else {
				// �������� �����־���
				m_vecKey[i].State = KEY_STATE::PRESSED;		// ������ ����
			}
			
			m_vecKey[i].PrevPressed = true;
		}
		else 
		{
			// �ش� KEY �� �ȴ����ִ�
			if (m_vecKey[i].PrevPressed == true)
			{
				// ���� frame���� �����־���
				m_vecKey[i].State = KEY_STATE::RELEASED;				// ���� �´�
			}
			else {
				// �������� �ȴ����־���
				m_vecKey[i].State = KEY_STATE::NONE;						// ��� �ȴ���
			}

			m_vecKey[i].PrevPressed = false;
		}
	}

	// ���콺 ��ǥ ����
	m_PrevMousePos = m_CurMousePos;										// ���� ���콺 ��ǥ�� �ű��
	POINT ptMouse = {};
	GetCursorPos(&ptMouse);				// ������ ���� ��ǥ
	ScreenToClient(CEngine::getInstance()->getMainWnd(), &ptMouse);				// ������ ���� ��ǥ -> ���ø����̼� Ŭ���̾�Ʈ ��ǥ�� ��ȯ
	m_CurMousePos = Vec2((float)ptMouse.x, (float)ptMouse.y);			// ��ǥ ���

	// ���콺 �̵��� ���
	m_DragDirection = m_CurMousePos - m_PrevMousePos;				// ���������� ���ϱ� ���� �̵����� ���Ѵ�
	m_DragDirection.y *= -1.f;					// ���� ���̴� �������� �����ؼ� �۾�
	m_DragDirection.Normalize();				// ���������� �ٲ۴�
}
