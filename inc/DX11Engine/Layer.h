#pragma once

#include "Entity.h"

// ���� ����
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

