#pragma once

#include "Entity.h"

// 전방 선언
class CGameObject;

class CLayer :
    public CEntity
{
private:
    std::vector<CGameObject*> m_vecObject;
public:
    CLayer();
    virtual ~CLayer();
};

