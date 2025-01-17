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

    void begin();
    void tick();
    void finaltick();
    void render();

    void addObject(CGameObject* _Object) { m_vecObject.push_back(_Object); }
};

