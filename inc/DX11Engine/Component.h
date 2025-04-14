#pragma once
#include "Entity.h"

#include "GameObject.h"

#define GET_OTHER_COMPONENT(Type)               C##Type* Type() {return m_owner->Type();}

class CComponent :
    public CEntity
{
private:
    CGameObject* m_owner;                       // �ش� ������Ʈ�� �����ϰ� �ִ� ������Ʈ�� �ּ�
    const COMPONENT_TYPE m_type;            // �ش� ��ü�� ������Ʈ Ÿ��

public:
    CComponent(COMPONENT_TYPE _type);
    virtual ~CComponent();

    virtual void begin() {};                // ���� �Լ�
    virtual void tick() {};                 // ���� �Լ�
    virtual void finaltick() = 0;           // ���� ���� �Լ�

    COMPONENT_TYPE getComponentType() { return m_type; }
    CGameObject* getOwner() { return m_owner; }
    //CTransform* getTransform() { return m_owner->getTransform(); }
    //CMeshRender* getMeshRender() { return m_owner->getMeshRender(); }
    //CCamera* getCamera() { return m_owner->getCamera(); }

    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(Collider2D);

    friend class CGameObject;               // CGameObject ���Ը� private�� ������

};

