#ifndef ENGINE_H_
#define ENGINE_H_

// 디자인패턴 : 설계유형

// 데이터 영역 메모리 : 전역 변수
// 프로세스 시작할 때 초기화
// 프로세스 종료시 해제

// 싱글턴패턴으로 엔진 구성
class CEngine
	: public CSingleton<CEngine>
{
	SINGLE(CEngine);
private:
	HWND m_hWnd;
	POINT m_resolution;

public:
	int init(HWND _hWnd, POINT _resolution);

	// 매 프레임마다 엔진에 들어오는 함수
	void progress();

	HWND getMainWnd() { return m_hWnd; }

public:
	// 정적 멤버함수로 만들어서 객체가 없어도 호출 가능하게 만든다
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
	//		g_this = nullptr;			// 삭제 후 nullptr을 대입한다
	//	}
	//}

};

#endif		ENGINE_H_