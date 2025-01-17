#pragma once
#include "singleton.h"

// 전방 선언
class CLevel;

class CLevelMgr
    : public CSingleton<CLevelMgr>
{
    SINGLE(CLevelMgr)
private:
    CLevel* m_curLevel;

public:
    void init();
    void tick();
    void render();
};

