#pragma once
#include "singleton.h"
class CAssetMgr :
    public CSingleton<CAssetMgr>
{
    SINGLE(CAssetMgr)
private:


public:
    void init();
    void createDefaultMesh();
    void createDefaultTexture();
    void createDefaultMaterial();
    void createDefaultGraphicShader();
    void createDefaultComputeShader();
};

