#pragma once
#include "RenderComponent.h"
class CMeshRender :
    public CRenderComponent
{
public:
    virtual void finaltick() override;
    virtual void render() override;

    CMeshRender();
    virtual ~CMeshRender();
};

