#pragma once

#include "Asset.h"

class CTexture
    : public CAsset
{
private:
    ScratchImage m_image;                                       // 시스템 메모리에서 불러온 텍스처 이미지를 관리한다
    ComPtr<ID3D11Texture2D> m_pTex2D;               // GPU로 보낸다
    ComPtr<ID3D11ShaderResourceView> m_SRV;

public:
    CTexture();
    virtual ~CTexture();

    void binding(int _registerNum);

private:
    virtual int load(const std::wstring& _strFilePath) override;
    virtual int save(const std::wstring& _strFilePath) override;

};

