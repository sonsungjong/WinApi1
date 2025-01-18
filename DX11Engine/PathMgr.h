#pragma once
#include "singleton.h"
class CPathMgr
	: public CSingleton<CPathMgr>
{
	SINGLE(CPathMgr)
private:
	wchar_t m_szResourcePath[255];

public:
	void init();
	const wchar_t* getResPath() { return m_szResourcePath; }
};

