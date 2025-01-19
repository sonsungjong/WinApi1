#pragma once
#include "Entity.h"

#include "GameObject.h"

class CComponent :
    public CEntity
{
private:
    CGameObject* m_owner;                       // 해당 컴포넌트를 소유하고 있는 오브젝트의 주소
    const COMPONENT_TYPE m_type;            // 해당 객체의 컴포넌트 타입

public:
    CComponent(COMPONENT_TYPE _type);
    virtual ~CComponent();

    virtual void begin() {};                // 가상 함수
    virtual void tick() {};                 // 가상 함수
    virtual void finaltick() = 0;           // 순수 가상 함수

    COMPONENT_TYPE getComponentType() { return m_type; }
    CGameObject* getOwner() { return m_owner; }
    CTransform* getTransform() { return m_owner->getTransform(); }
    CMeshRender* getMeshRender() { return m_owner->getMeshRender(); }
    CCamera* getCamera() { return m_owner->getCamera(); }

    friend class CGameObject;               // CGameObject 에게만 private을 열어줌

};

