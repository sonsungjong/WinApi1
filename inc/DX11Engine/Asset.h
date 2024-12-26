#pragma once
#include "Entity.h"
#include "enum.h"

// Asset(�ڿ�)�� �ش��ϴ� ��� Ŭ������ ����κ��� �ִ´�

class CAsset 
    : public CEntity
{
private:
    std::wstring m_key;                             // �ε��� Ű��
    std::wstring m_relativePath;                // �����

    const ASSET_TYPE m_type;                // Asset�� Ÿ��

public:
    const std::wstring& getKey() { return m_key; }
    const std::wstring& getRelativePath() { return m_relativePath; }
    ASSET_TYPE getAssetType() { return m_type; }

    CAsset(ASSET_TYPE _type);
    ~CAsset();
};

