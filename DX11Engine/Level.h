#pragma once
#include "Entity.h"

// ���� ����
class CLayer;

// ���ϴ� �׷�(���̾�) ������ ����
class CLevel :
    public CEntity
{
private:
    CLayer* m_arrLayer[MAX_LAYER];          // 32 ����


public:
    CLevel();
    virtual ~CLevel();

};

