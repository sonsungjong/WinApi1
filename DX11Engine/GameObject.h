#pragma once
#include "Entity.h"

// ���� ����
class CComponent;
class CRenderComponent;
class CTransform;
class CMeshRender;

class CScript;

class CGameObject :
    public CEntity
{
private:
    CComponent* m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent* m_RenderCom;

    // ��ũ��Ʈ ������Ʈ�� �迭�� �ƴ϶� ������ ���� �߰��� �� �ְ� ���ͷ� ����
    std::vector< CScript*> m_vecScripts;

public:
    void begin();               // ������ ���۵� ��, ������ �շ��� �� ȣ��Ǵ� �Լ�
    void tick();                // �� �����Ӹ��� ȣ��Ǵ� �Լ� (DeltaTime ���� �� ���� ����)
    void finaltick();           // tick���� �߻��� �ൿ�� ������ or ���ҽ� ���ε� �� ���� ������ ����
    void render();              // ȭ�鿡 �׸��� �Լ�

public:
    void addComponent(CComponent* _component);
    CComponent* getComponent(COMPONENT_TYPE _type);                 // �ڽĿ��� ĳ�����ؾ���

    CTransform* getTransform() { return (CTransform*)m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM]; }
    CMeshRender* getMeshRender() { return (CMeshRender*)m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER]; }

public:
    CGameObject();
    virtual ~CGameObject();
};

// ������Ʈ ��� ����