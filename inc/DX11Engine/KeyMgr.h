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

public:
	void init();
	void tick();

public:
	KEY_STATE getKeyState(KEY _Key) { return m_vecKey[(UINT)_Key].State; }
};

