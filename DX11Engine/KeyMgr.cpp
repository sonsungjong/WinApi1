#include "pch.h"
#include "KeyMgr.h"

#include "Engine.h"

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

	// 마우스 좌표 갱신
	m_PrevMousePos = m_CurMousePos;										// 이전 마우스 좌표로 옮기고
	POINT ptMouse = {};
	GetCursorPos(&ptMouse);				// 윈도우 기준 좌표
	ScreenToClient(CEngine::getInstance()->getMainWnd(), &ptMouse);				// 윈도우 기준 좌표 -> 애플리케이션 클라이언트 좌표로 변환
	m_CurMousePos = Vec2((float)ptMouse.x, (float)ptMouse.y);			// 좌표 계산

	// 마우스 이동값 계산
	m_DragDirection = m_CurMousePos - m_PrevMousePos;				// 방향정보를 구하기 위해 이동값을 구한다
	m_DragDirection.y *= -1.f;					// 눈에 보이는 방향으로 변경해서 작업
	m_DragDirection.Normalize();				// 단위값으로 바꾼다
}
