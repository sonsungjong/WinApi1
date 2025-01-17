#include "pch.h"
#include "Level.h"

#include "Layer.h"

CLevel::CLevel()
	: m_arrLayer{}
{
	// 32개 모두 객체 생성
	for (UINT i = 0U; i < MAX_LAYER; ++i) {
		m_arrLayer[i] = new CLayer;
	}
}

CLevel::~CLevel()
{
	SafeDelArray(m_arrLayer);				// 배열 일괄 해제
}
