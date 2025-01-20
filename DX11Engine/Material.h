#pragma once
#include "Asset.h"

#include "assets.h"

class CMaterial
    : public CAsset
{
private:
    Ptr<CGraphicShader> m_shader;

    tMtrlConst m_const;
    Ptr<CTexture> m_arrTex[TEX_PARAM::END];

public:
    CMaterial();
    virtual ~CMaterial();

    void setShader(Ptr<CGraphicShader> _shader) { m_shader = _shader; }
    Ptr<CGraphicShader> getShader() { return m_shader; }

    void binding();

    virtual int load(const std::wstring& _strFilePath) { return S_OK; }
    virtual int save(const std::wstring& _strFilePath) { return S_OK; }
};

