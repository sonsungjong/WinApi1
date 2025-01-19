#pragma once

#include "singleton.h"

class CCamera;

// 렌더링 관련
class CRenderMgr :
    public CSingleton<CRenderMgr>
{
    SINGLE(CRenderMgr);
private:
    std::vector<CCamera*> m_vecCam;             // 카메라 컴포넌트를 보유하고 있는 오브젝트가 여러개일 수 있어서 벡터로 (0번이 메인카메라)

public:
    void init();
    void tick();
    void render();

    void registerCamera(CCamera* _Cam, int _priority);
};

