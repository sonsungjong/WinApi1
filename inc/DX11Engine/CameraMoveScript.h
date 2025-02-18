#pragma once
#include "Script.h"
class CCameraMoveScript
    : public CScript
{
private:
    float m_speed;

    void MoveByPerspective();
    void MoveByOrthographic();

public:
    virtual void tick() override;

    CCameraMoveScript();
    virtual ~CCameraMoveScript();
};

