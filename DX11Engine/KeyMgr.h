#pragma once
#include "singleton.h"

// �켱�� ������ Ű�Է�����, ���Ŀ� ���̷�Ʈ ��ǲ���� ����
// Ű �Է� ����ȭ

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
	R,
	T,
	Y,
	U,
	I,
	O,
	P,
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
	NUM0,
	NUM1,
	NUM2,
	NUM3,
	NUM4,
	NUM5,
	NUM6,
	NUM7,
	NUM8,
	NUM9,
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
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	KEY_END
};

enum class KEY_STATE
{
	TAP,						// ����
	PRESSED,				// ������ ��
	RELEASED,			// ��
	NONE,					// �ȴ����� ��
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
	std::vector<STKeyInfo> m_vecKey;			// ���� �ε����� Ű�� enum������
	Vec2 m_CurMousePos;								// ���� ������ ���콺 ��ġ
	Vec2 m_PrevMousePos;							// ���� ������ ���콺 ��ġ
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

