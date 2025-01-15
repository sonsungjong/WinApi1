#pragma once
#include "Entity.h"
class CComponent :
    public CEntity
{
private:
    const COMPONENT_TYPE m_type;

public:
    CComponent(COMPONENT_TYPE _type);
    virtual ~CComponent();
};

