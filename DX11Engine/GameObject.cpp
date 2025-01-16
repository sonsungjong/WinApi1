#include "pch.h"
#include "GameObject.h"

#include "Component.h"
#include "RenderComponent.h"
CGameObject::CGameObject()
	: m_arrCom{}
	, m_RenderCom(nullptr)
{
}

CGameObject::~CGameObject()
{
	SafeDelArray(m_arrCom);
}


void CGameObject::begin()
{
	// �����ϰ� �ִ� ��� ������Ʈ�鿡�� begin�� ȣ��
	for (UINT i = 0U; i < (UINT)COMPONENT_TYPE::END; ++i) {
		if (m_arrCom[i]) {
			m_arrCom[i]->begin();
		}
	}
}

void CGameObject::tick()
{
	for (UINT i = 0U; i < (UINT)COMPONENT_TYPE::END; ++i) {
		if (m_arrCom[i]) {
			m_arrCom[i]->tick();
		}
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
	// �������� �ϳ��� ������Ʈ������ �ϰ�ó��
	if (m_RenderCom) {
		m_RenderCom->render();
	}
}

void CGameObject::addComponent(CComponent* _component)
{
	// ������Ʈ�� Ÿ���� Ȯ��
	COMPONENT_TYPE type = _component->getComponentType();

	// �Է����� ���� ������Ʈ�� �̹� ������ �ִ� ��� [���]
	assert(!m_arrCom[(UINT)type]);

	// ������Ʈ�� ������ �������� �˻�
	CRenderComponent* pRenderCom = dynamic_cast<CRenderComponent*>(_component);
	if (nullptr != pRenderCom) {
		// �̹� ������ ���� ������Ʈ�� ������ ���
		assert(!m_RenderCom);
		m_RenderCom = pRenderCom;
	}

	// �Էµ� ������Ʈ�� �迭�� �˸��� �ε��� �ڸ��� �ּҰ��� ����Ѵ�.
	m_arrCom[(UINT)type] = _component;

	// �ش� ������Ʈ�� ���� ������Ʈ�� �̰����� �˷���
	_component->m_owner = this;
}

CComponent* CGameObject::getComponent(COMPONENT_TYPE _type)
{
	return m_arrCom[(UINT)_type];
}

