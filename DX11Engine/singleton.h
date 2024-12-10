#pragma once

// �� �̱��� Ŭ������ ����ϴ°� �����Ŀ� ���� �ڷ����� ������ ����

template<typename T>
class CSingleton
{
private:
	typedef void(*DESTROY)(void);				// �Լ������� Ÿ��

private:
	static T* m_this;			// �� �ڽ��� ����Ű�� this ������

public:
	static T* getInstance()
	{
		if (nullptr == m_this)
		{
			m_this = new T;				// �� �޸�
		}
		return m_this;
	}

	static void destroy()
	{
		if (nullptr != m_this)
		{
			delete m_this;
			m_this = nullptr;
		}
	}

protected:
	CSingleton()
	{
		atexit((DESTROY)&CSingleton<T>::destroy);			// atexit : ��ȯ Ÿ���� void�̰� ���ڰ� void�� �Լ��� �ּҸ� �־������ ���α׷��� ����ɶ� �ش� �Լ������͸� ȣ���Ų��
	}
	CSingleton(const CSingleton& _other) = delete;
	virtual ~CSingleton()
	{

	}

};

// ��������� �ʱ�ȭ�� ������Ѵ�
template<typename T>
T* CSingleton<T>::m_this = nullptr;