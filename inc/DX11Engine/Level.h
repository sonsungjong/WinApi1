#pragma once
#include "Entity.h"

// ���� ����
class CLayer;
class CGameObject;

// ���ϴ� �׷�(���̾�) ������ ����
class CLevel :
    public CEntity
{
private:
    CLayer* m_arrLayer[MAX_LAYER];          // 32 ����

public:
    CLevel();
    virtual ~CLevel();

    void begin();
    void tick();
    void finaltick();
    void render();

    void addObject(UINT _LayerIdx, CGameObject* _Object);
};

