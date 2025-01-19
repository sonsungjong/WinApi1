#include "pch.h"
#include "KeyMgr.h"

// enum과 실제 키보드 아스키값을 매칭시켜줄 전역 배열
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
	VK_RETURN,			// 엔터 키
	VK_ESCAPE,			// ECS 키
	VK_SPACE,
	VK_LSHIFT,
	VK_MENU,				// ALT 키
	VK_CONTROL,		// CTRL 키
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
			// KEY enum값에 해당하는 값이 눌리면
			if (m_vecKey[i].PrevPressed == false)
			{
				// 이전 frame에는 안눌려 있었다
				m_vecKey[i].State = KEY_STATE::TAP;				// 처음 눌림
			}
			else {
				// 이전에도 눌려있었다
				m_vecKey[i].State = KEY_STATE::PRESSED;		// 눌리고 있음
			}
			
			m_vecKey[i].PrevPressed = true;
		}
		else 
		{
			// 해당 KEY 가 안눌려있다
			if (m_vecKey[i].PrevPressed == true)
			{
				// 이전 frame에도 눌려있었다
				m_vecKey[i].State = KEY_STATE::RELEASED;				// 이제 뗏다
			}
			else {
				// 이전에도 안눌려있었음
				m_vecKey[i].State = KEY_STATE::NONE;						// 계속 안눌림
			}

			m_vecKey[i].PrevPressed = false;
		}
	}
}
