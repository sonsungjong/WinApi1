#pragma once
#include "Entity.h"

// 전방 선언
class CComponent;
class CRenderComponent;
class CTransform;
class CMeshRender;
class CCamera;
class CCollider2D;

class CScript;

#define GET_COMPONENT(Type, TYPE) C##Type* Type() {return (C##Type*)m_arrCom[(UINT)COMPONENT_TYPE::TYPE];}

class CGameObject :
    public CEntity
{
private:
    CComponent* m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent* m_RenderCom;

    // 스크립트 컴포넌트는 배열이 아니라 무수히 많이 추가할 수 있게 벡터로 관리
    std::vector< CScript*> m_vecScripts;

    CGameObject* m_Parent;                                      // 부모 오브젝트
    std::vector<CGameObject*> m_vecChild;               // 소유한 자식 오브젝트

public:
    void begin();               // 레벨이 시작될 때, 레벨에 합류할 때 호출되는 함수
    void tick();                // 매 프레임마다 호출되는 함수 (DeltaTime 동안 할 일을 구현)
    void finaltick();           // tick에서 발생한 행동을 마무리 or 리소스 바인딩 및 관련 데이터 연산
    void render();              // 화면에 그리는 함수

public:
    void addComponent(CComponent* _component);
    CComponent* getComponent(COMPONENT_TYPE _type);                 // 자식에서 캐스팅해야함

    GET_COMPONENT(Transform, TRANSFORM);
    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(Camera, CAMERA);
    GET_COMPONENT(Collider2D, COLLIDER2D);
    //CTransform* getTransform() { return (CTransform*)m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM]; }
    //CMeshRender* getMeshRender() { return (CMeshRender*)m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER]; }
    //CCamera* getCamera() { return (CCamera*)m_arrCom[(UINT)COMPONENT_TYPE::CAMERA]; }

public:
    CGameObject();
    virtual ~CGameObject();

    void addChild(CGameObject* _object);
};

// 컴포넌트 기반 설계