#pragma once
#include "Script.h"


class CPlayerScript :
    public CScript
{
private:
    float m_speed;

public:
    virtual void tick() override;

    CPlayerScript();
    virtual ~CPlayerScript();

};

