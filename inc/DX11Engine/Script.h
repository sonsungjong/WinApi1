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

    virtual void tick() = 0;        // 순수 가상 함수로 만들어서 자식이 꼭 구현하게끔 강제한다

    // final을 붙여서 더이상 자식이 오버라이딩할 수 없게 만든다
    virtual void finaltick() final
    {
    }
};

