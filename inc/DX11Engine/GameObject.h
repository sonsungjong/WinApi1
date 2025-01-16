#pragma once
#include "Entity.h"

// 전방 선언
class CComponent;
class CRenderComponent;
class CTransform;
class CMeshRender;

class CGameObject :
    public CEntity
{
private:
    CComponent* m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent* m_RenderCom;

public:
    void begin();               // 레벨이 시작될 때, 레벨에 합류할 때 호출되는 함수
    void tick();                // 매 프레임마다 호출되는 함수 (DeltaTime 동안 할 일을 구현)
    void finaltick();           // tick에서 발생한 행동을 마무리 or 리소스 바인딩 및 관련 데이터 연산
    void render();              // 화면에 그리는 함수

public:
    void addComponent(CComponent* _component);
    CComponent* getComponent(COMPONENT_TYPE _type);                 // 자식에서 캐스팅해야함

    CTransform* getTransform() { return (CTransform*)m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM]; }
    CMeshRender* getMeshRender() { return (CMeshRender*)m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER]; }

public:
    CGameObject();
    virtual ~CGameObject();
};

// 컴포넌트 기반 설계