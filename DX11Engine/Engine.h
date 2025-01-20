#ifndef ENGINE_H_
#define ENGINE_H_

// FBX Autodesk FBX SDK) : FBX ���� ���� SDK [.FBX : ���� �� �ִϸ��̼�]
// Assimp (Open Asset Import Library) : OBJ, FBX, DAE ���� DX11�� �Բ� ��� ���� [.OBJ : ���� ��]

// ���������� : ��������

// ������ ���� �޸� : ���� ����
// ���μ��� ������ �� �ʱ�ȭ
// ���μ��� ����� ����

// �̱����������� ���� ����
class CEngine
	: public CSingleton<CEngine>
{
	SINGLE(CEngine);
private:
	HWND m_hWnd;
	Vec2 m_resolution;

public:
	int init(HWND _hWnd, Vec2 _resolution);

	// �� �����Ӹ��� ������ ������ �Լ�
	void progress();

	HWND getMainWnd() { return m_hWnd; }

public:
	// ���� ����Լ��� ���� ��ü�� ��� ȣ�� �����ϰ� �����
	//static CEngine* getInstance()
	//{
	//	if (g_this == nullptr) {
	//		g_this = new CEngine;
	//	}
	//	return g_this;
	//}

	//static void destroy()
	//{
	//	if (g_this != nullptr) {
	//		delete g_this;
	//		g_this = nullptr;			// ���� �� nullptr�� �����Ѵ�
	//	}
	//}

};

#endif		ENGINE_H_