#include "pch.h"
#include "Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
	SafeDelVector(m_vecObject);
}

void CLayer::begin()
{
	for (size_t i = 0U; i < m_vecObject.size(); ++i) {
		m_vecObject[i]->begin();
	}
}

void CLayer::tick()
{
	for (size_t i = 0U; i < m_vecObject.size(); ++i) {
		m_vecObject[i]->tick();
	}
}

void CLayer::finaltick()
{
	for (size_t i = 0U; i < m_vecObject.size(); ++i) {
		m_vecObject[i]->finaltick();
	}
}

void CLayer::render()
{
	for (size_t i = 0U; i < m_vecObject.size(); ++i) {
		m_vecObject[i]->render();
	}
}
