#include "pch.h"
#include "GameObject.h"

#include "Component.h"
#include "RenderComponent.h"
#include "Script.h"

CGameObject::CGameObject()
	: m_arrCom{}
	, m_RenderCom(nullptr)
	, m_Parent(nullptr)
{
}

CGameObject::~CGameObject()
{
	SafeDelArray(m_arrCom);
}

void CGameObject::addChild(CGameObject* _object)
{
	_object->m_Parent = this;
	m_vecChild.push_back(_object);
}


void CGameObject::begin()
{
	// 소유하고 있는 모든 컴포넌트들에게 begin을 호출
	for (UINT i = 0U; i < (UINT)COMPONENT_TYPE::END; ++i) {
		if (m_arrCom[i]) {
			m_arrCom[i]->begin();
		}
	}

	for (size_t i = 0U; i < m_vecScripts.size(); ++i) {
		m_vecScripts[i]->begin();
	}
}

void CGameObject::tick()
{
	for (UINT i = 0U; i < (UINT)COMPONENT_TYPE::END; ++i) {
		if (m_arrCom[i]) {
			m_arrCom[i]->tick();
		}
	}

	for (size_t i = 0U; i < m_vecScripts.size(); ++i) {
		m_vecScripts[i]->tick();
	}
}

void CGameObject::finaltick()
{
	for (UINT i = 0U; i < (UINT)COMPONENT_TYPE::END; ++i) {
		if (m_arrCom[i]) {
			m_arrCom[i]->finaltick();
		}
	}
}

void CGameObject::render()
{
	// 렌더링은 하나의 컴포넌트에서만 일괄처리
	if (m_RenderCom) {
		m_RenderCom->render();
	}
}

void CGameObject::addComponent(CComponent* _component)
{
	// 컴포넌트의 타입을 확인
	COMPONENT_TYPE type = _component->getComponentType();

	if (type == COMPONENT_TYPE::SCRIPT)
	{
		// 스크립트 컴포넌트
		m_vecScripts.push_back((CScript*)_component);
	}
	else
	{
		// 입력으로 들어온 컴포넌트를 이미 가지고 있는 경우 [경고]
		assert(!m_arrCom[(UINT)type]);

		// 컴포넌트가 렌더링 관련인지 검사
		CRenderComponent* pRenderCom = dynamic_cast<CRenderComponent*>(_component);
		if (nullptr != pRenderCom) {
			// 이미 렌더링 관련 컴포넌트를 보유한 경우
			assert(!m_RenderCom);
			m_RenderCom = pRenderCom;
		}

		// 입력된 컴포넌트를 배열의 알맞은 인덱스 자리에 주소값을 기록한다.
		m_arrCom[(UINT)type] = _component;
	}
	// 해당 컴포넌트의 소유 오브젝트가 이것임을 알려줌
	if (_component) {
		_component->m_owner = this;
	}
}

CComponent* CGameObject::getComponent(COMPONENT_TYPE _type)
{
	return m_arrCom[(UINT)_type];
}

