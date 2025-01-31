#pragma once
#include "singleton.h"

class CGameObject;

class CDbgRenderMgr :
    public CSingleton<CDbgRenderMgr>
{
    SINGLE(CDbgRenderMgr);

private:
    std::list<tDebugShapeInfo> m_lstShapeInfo;                              // 중간 삭제가 가능한 list
    CGameObject* m_DebugRenderObj;

public:
    void AddDebugShapeInfo(const tDebugShapeInfo& _info);

    void render();

private:
    void calculateWorldMatrix(tDebugShapeInfo& _info);

};

