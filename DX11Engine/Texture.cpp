#include "pch.h"
#include "Texture.h"

#include "Device.h"

CTexture::CTexture()
    : CAsset(ASSET_TYPE::TEXTURE)
{
}

CTexture::~CTexture()
{
}

int CTexture::load(const std::wstring& _strFilePath)
{
    // Ȯ���ڿ� ���� �б�ó���� �Ѵ�
    wchar_t Ext[50]{0};                                 // Ȯ���ڸ� ���� �迭
    _wsplitpath_s(_strFilePath.c_str(), NULL, 0, NULL, 0, NULL, 0, Ext, 50);

    std::wstring strExt = Ext;

    HRESULT hr = E_FAIL;
    // Ȯ���� �� ���� �˸°� ��� (DirectXTex)
    if ((strExt == L".dds") || (strExt == L".DDS"))
    {
        hr = LoadFromDDSFile(_strFilePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_image);              // .dds    .DDS
    }
    else if ((strExt == L".tga") || (strExt == L".TGA"))
    {
        hr = LoadFromTGAFile(_strFilePath.c_str(), nullptr, m_image);              // .tga    .TGA
    }
    else
    {
        hr = LoadFromWICFile(_strFilePath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, m_image);              // .png    .jpg    .jpeg    .bmp
    }

    if (SUCCEEDED(hr)) {
        // Texture2D �� ShaderResourceView ����
        hr = CreateShaderResourceView(DEVICE, m_image.GetImages(), m_image.GetImageCount(), m_image.GetMetadata(), m_SRV.GetAddressOf());

        if (SUCCEEDED(hr)) {
            m_SRV->GetResource((ID3D11Resource**)m_pTex2D.GetAddressOf());
        }
        else {
            MessageBox(NULL, L"ShaderResourceView ���� ����", L"ERROR", MB_OK);
        }
    }
    else {
        MessageBox(NULL, L"�ؽ�ó �ε� ����", L"ERROR", MB_OK);
    }

    return hr;
}

int CTexture::save(const std::wstring& _strFilePath)
{
    return 0;
}
