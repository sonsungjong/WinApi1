#pragma once
#include "Script.h"
class CCameraMoveScript
    : public CScript
{
private:
    float m_speed;

public:
    virtual void tick() override;

    CCameraMoveScript();
    virtual ~CCameraMoveScript();
};

