#pragma once
#include "singleton.h"

// 우선은 윈도우 키입력으로, 추후에 다이렉트 인풋으로 변경
// 키 입력 동기화

enum class KEY
{
	W,
	S,
	A,
	D,
	Z,
	X,
	C,
	V,
	Y,
	_0,
	_1,
	_2,
	_3,
	_4,
	_5,
	_6,
	_7,
	_8,
	_9,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	LBTN,
	RBTN,
	ENTER,
	ESC,
	SPACE,
	LSHIFT,
	ALT,
	CTRL,

	KEY_END
};

enum class KEY_STATE
{
	TAP,						// 눌림
	PRESSED,				// 눌리는 중
	RELEASED,			// 뗌
	NONE,					// 안누르는 중
};

struct STKeyInfo
{
	KEY_STATE State;
	bool PrevPressed;
};

class CKeyMgr
	: public CSingleton<CKeyMgr>
{
	SINGLE(CKeyMgr);

private:
	std::vector<STKeyInfo> m_vecKey;			// 벡터 인덱스는 키의 enum값으로
	Vec2 m_CurMousePos;								// 현재 프레임 마우스 위치
	Vec2 m_PrevMousePos;							// 이전 프레임 마우스 위치
	Vec2 m_DragDirection;

public:
	void init();
	void tick();

public:
	KEY_STATE getKeyState(KEY _Key) { return m_vecKey[(UINT)_Key].State; }

	Vec2 getMousePos() { return m_CurMousePos; }
	Vec2 getPrevMousePos() { return m_PrevMousePos; }
	Vec2 getMouseDrag() { return m_DragDirection; }
};

