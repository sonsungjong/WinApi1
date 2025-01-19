#pragma once

#include "singleton.h"

class CCamera;

// ������ ����
class CRenderMgr :
    public CSingleton<CRenderMgr>
{
    SINGLE(CRenderMgr);
private:
    std::vector<CCamera*> m_vecCam;             // ī�޶� ������Ʈ�� �����ϰ� �ִ� ������Ʈ�� �������� �� �־ ���ͷ� (0���� ����ī�޶�)

public:
    void init();
    void tick();
    void render();

    void registerCamera(CCamera* _Cam, int _priority);
};

