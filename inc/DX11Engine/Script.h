#pragma once
#include "Component.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "AssetMgr.h"

#include "Transform.h"

class CScript :
    public CComponent
{
private:

public:
    CScript();
    virtual ~CScript();

    virtual void tick() = 0;        // ���� ���� �Լ��� ���� �ڽ��� �� �����ϰԲ� �����Ѵ�

    // final�� �ٿ��� ���̻� �ڽ��� �������̵��� �� ���� �����
    virtual void finaltick() final
    {
    }
};

