#ifndef ENGINE_H_
#define ENGINE_H_

// ���������� : ��������

// ������ ���� �޸� : ���� ����
// ���μ��� ������ �� �ʱ�ȭ
// ���μ��� ����� ����

// �̱����������� ���� ����
class CEngine
{
private:
	HWND m_hWnd;
	POINT m_resolution;

public:
	int init(HWND _hWnd, POINT _resolution);


public:
	// ���� ����Լ��� ���� ��ü�� ��� ȣ�� �����ϰ� �����
	static CEngine* getInstance()
	{
		if (g_this == nullptr) {
			g_this = new CEngine;
		}
		return g_this;
	}

	static void destroy()
	{
		if (g_this != nullptr) {
			delete g_this;
			g_this = nullptr;			// ���� �� nullptr�� �����Ѵ�
		}
	}

private:
	// �����ڸ� private�� �ּ� �ܺο��� ��ü������ �Ұ����ϰ�
	CEngine();
	virtual ~CEngine();
	CEngine(const CEngine& _origin) = delete;
	CEngine operator=(const CEngine& _origin) = delete;

	static CEngine* g_this;
};

#endif		ENGINE_H_