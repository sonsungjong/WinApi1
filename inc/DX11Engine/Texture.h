#pragma once

#include "Asset.h"

class CTexture
    : public CAsset
{
private:
    ScratchImage m_image;                                       // �ý��� �޸𸮿��� �ҷ��� �ؽ�ó �̹����� �����Ѵ�
    ComPtr<ID3D11Texture2D> m_pTex2D;               // GPU�� ������
    ComPtr<ID3D11ShaderResourceView> m_SRV;

public:
    CTexture();
    virtual ~CTexture();

    void binding(int _registerNum);

private:
    virtual int load(const std::wstring& _strFilePath) override;
    virtual int save(const std::wstring& _strFilePath) override;

};

