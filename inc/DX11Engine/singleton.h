#pragma once

// 이 싱글톤 클래스를 상속하는게 누구냐에 따라서 자료형을 정해줄 것임

template<typename T>
class CSingleton
{
private:
	typedef void(*DESTROY)(void);				// 함수포인터 타입

private:
	static T* m_this;			// 나 자신을 가리키는 this 포인터

public:
	static T* getInstance()
	{
		if (nullptr == m_this)
		{
			m_this = new T;				// 힙 메모리
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
		atexit((DESTROY)&CSingleton<T>::destroy);			// atexit : 반환 타입이 void이고 인자가 void인 함수의 주소를 넣어놓으면 프로그램이 종료될때 해당 함수포인터를 호출시킨다
	}
	CSingleton(const CSingleton& _other) = delete;
	virtual ~CSingleton()
	{

	}

};

// 정적멤버는 초기화를 해줘야한다
template<typename T>
T* CSingleton<T>::m_this = nullptr;