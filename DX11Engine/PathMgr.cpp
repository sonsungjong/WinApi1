#include "pch.h"
#include "PathMgr.h"

CPathMgr::CPathMgr()
	: m_szResourcePath{}
{

}

CPathMgr::~CPathMgr()
{

}

void CPathMgr::init()
{
	// 현재 디렉토리의 경로를 얻어온다
	GetCurrentDirectory(255, m_szResourcePath);

	size_t len = wcslen(m_szResourcePath);

	// 폴더를 하나 나가는 로직
	//for (size_t i = len - 1U; i > 0U; --i) 
	//{
	//	if (m_szContentPath[i] == '\\') 
	//	{
	//		m_szContentPath[i] = '\0';
	//		break;
	//	}
	//}

	// 현재 실행경로에서 res폴더를 들어가는 경로
	wcscat_s(m_szResourcePath, L"\\res\\");
}
