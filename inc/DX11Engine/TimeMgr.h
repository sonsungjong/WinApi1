#pragma once
#include "singleton.h"

// �ð�����ȭ �� ����� DeltaTime ���� ���� (�����Ӱ� �ð� ����)

class CTimeMgr
	: public CSingleton<CTimeMgr>
{
	SINGLE(CTimeMgr)
private:
	LARGE_INTEGER m_llFrequency;			// �ʴ� ī��Ʈ ��
	LARGE_INTEGER m_llCurCount;			// ���� ī��Ʈ ��
	LARGE_INTEGER m_llPrevCount;			// ���� ������ ī��Ʈ ��

	float m_DT;					// Delta Time : 1�����ӵ����ϴµ� �ɸ� �ð�
	float m_time;					// ���μ��� ���� ������ ���� �ð�
	float m_accTime;			// 1�ʸ� üũ�ϱ� ���� �ð� ��������

	UINT m_FrmCount;

public:
	void init();
	void tick();				// �����Ӹ��� ������ �Լ�

public:
	float getDeltaTime()
	{
		return m_DT;
	}
};

