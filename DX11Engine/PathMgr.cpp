#include "pch.h"
#include "PathMgr.h"

CPathMgr::CPathMgr()
	: m_szContentPath{}
{

}

CPathMgr::~CPathMgr()
{

}

void CPathMgr::init()
{
	// 현재 디렉토리의 경로를 얻어온다
	GetCurrentDirectory(255, m_szContentPath);

	size_t len = wcslen(m_szContentPath);

	for (size_t i = len - 1U; i > 0U; --i) 
	{
		if (m_szContentPath[i] == '\\') 
		{
			m_szContentPath[i] = '\0';
			break;
		}
	}

	wcscat_s(m_szContentPath, L"\\content\\");
}
