#include "pch.h"
#include "KeyMgr.h"

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
	'_0',
	'_1',
	'_2',
	'_3',
	'_4',
	'_5',
	'_6',
	'_7',
	'_8',
	'_9',
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,
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
}
