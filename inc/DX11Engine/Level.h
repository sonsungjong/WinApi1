#pragma once
#include "Entity.h"

// 전방 선언
class CLayer;

// 원하는 그룹(레이어) 단위로 나눔
class CLevel :
    public CEntity
{
private:
    CLayer* m_arrLayer[MAX_LAYER];          // 32 계층


public:
    CLevel();
    virtual ~CLevel();

};

