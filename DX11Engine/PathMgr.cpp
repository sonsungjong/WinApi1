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
	// ���� ���丮�� ��θ� ���´�
	GetCurrentDirectory(255, m_szResourcePath);

	size_t len = wcslen(m_szResourcePath);

	// ������ �ϳ� ������ ����
	//for (size_t i = len - 1U; i > 0U; --i) 
	//{
	//	if (m_szContentPath[i] == '\\') 
	//	{
	//		m_szContentPath[i] = '\0';
	//		break;
	//	}
	//}

	// ���� �����ο��� res������ ���� ���
	wcscat_s(m_szResourcePath, L"\\res\\");
}
