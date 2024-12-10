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
	QueryPerformanceFrequency(&m_llFrequency);				// �ʴ� ī��Ʈ ���� �˷��༭ ���� üũ�� �ϰ��Ѵ�
	QueryPerformanceCounter(&m_llCurCount);						// PC�� ���� �������� ī�����ϰ� �ִ� �ð�
	m_llPrevCount = m_llCurCount;
}

void CTimeMgr::tick()
{
	QueryPerformanceCounter(&m_llCurCount);

	m_DT = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequency.QuadPart;			// ������ ���� �ð� ���

	m_llPrevCount = m_llCurCount;

	m_time += m_DT;				// ���α׷� ���� �ð�

	m_accTime += m_DT;
	++m_FrmCount;

	if (1.f < m_accTime)
	{
		// 1�ʿ� �ѹ��� ����
		HWND hMainWnd = CEngine::getInstance()->getMainWnd();

		wchar_t szText[255] = {};
		swprintf_s(szText, L"FPS : %d, DeltaTime : %f", m_FrmCount, m_DT);

		SetWindowText(hMainWnd, szText);

		m_accTime -= 1.f;
		m_FrmCount = 0;
	}
}
