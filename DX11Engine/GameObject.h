#pragma once
#include "Entity.h"

// 전방 선언
class CComponent;

class CGameObject :
    public CEntity
{
private:
    CComponent* m_arrCom[(UINT)COMPONENT_TYPE::END];

public:

};

// 컴포넌트 기반 설계