#include "pch.h"
#include "TimeMgr.h"

#include "Engine.h"

CTimeMgr::CTimeMgr()
	: m_llFrequency{}
	, m_llCurCount{}
	, m_llPrevCount{}
	, m_time(0.f)
	, m_DT(0.f)
	, m_accTime(0.f)
	, m_FrmCount(0)
{

}

CTimeMgr::~CTimeMgr()
{

}

void CTimeMgr::init()
{
	QueryPerformanceFrequency(&m_llFrequency);				// 초당 카운트 수를 알려줘서 성능 체크를 하게한다
	QueryPerformanceCounter(&m_llCurCount);						// PC가 켜진 다음부터 카운팅하고 있는 시간
	m_llPrevCount = m_llCurCount;
}

void CTimeMgr::tick()
{
	QueryPerformanceCounter(&m_llCurCount);

	m_DT = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequency.QuadPart;			// 프레임 간격 시간 계산

	m_llPrevCount = m_llCurCount;

	m_time += m_DT;				// 프로그램 누적 시간

	m_accTime += m_DT;
	++m_FrmCount;

	if (1.f < m_accTime)
	{
		// 1초에 한번씩 들어옴
		HWND hMainWnd = CEngine::getInstance()->getMainWnd();

		wchar_t szText[255] = {};
		swprintf_s(szText, L"FPS : %d, DeltaTime : %f", m_FrmCount, m_DT);

		SetWindowText(hMainWnd, szText);

		m_accTime -= 1.f;
		m_FrmCount = 0;
	}
}
