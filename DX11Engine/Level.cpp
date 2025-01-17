#include "pch.h"
#include "Level.h"

#include "Layer.h"

CLevel::CLevel()
	: m_arrLayer{}
{
	// 32�� ��� ��ü ����
	for (UINT i = 0U; i < MAX_LAYER; ++i) {
		m_arrLayer[i] = new CLayer;
	}
}

CLevel::~CLevel()
{
	SafeDelArray(m_arrLayer);				// �迭 �ϰ� ����
}
