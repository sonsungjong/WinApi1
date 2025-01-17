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

void CLevel::begin()
{
	for (UINT i = 0U; i < MAX_LAYER; ++i) {
		m_arrLayer[i]->begin();
	}
}

void CLevel::tick()
{
	for (UINT i = 0U; i < MAX_LAYER; ++i) {
		m_arrLayer[i]->tick();
	}
}

void CLevel::finaltick()
{
	for (UINT i = 0U; i < MAX_LAYER; ++i) {
		m_arrLayer[i]->finaltick();
	}
}

void CLevel::render()
{
	for (UINT i = 0U; i < MAX_LAYER; ++i) {
		m_arrLayer[i]->render();
	}
}

void CLevel::addObject(UINT _LayerIdx, CGameObject* _Object)
{
	
	m_arrLayer[_LayerIdx]->addObject(_Object);
	
}
