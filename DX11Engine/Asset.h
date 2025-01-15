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

    int m_nRefCount;                                // ���� ī��Ʈ

    void addRef() { m_nRefCount++; }
    void release() { 
        m_nRefCount--;
        if (m_nRefCount <= 0) {
            delete this;                // �����θ� ���� (�Ҹ��� ȣ��)
        }
    }

public:
    const std::wstring& getKey() { return m_key; }
    const std::wstring& getRelativePath() { return m_relativePath; }
    ASSET_TYPE getAssetType() { return m_type; }

    CAsset(ASSET_TYPE _type);
    ~CAsset();
    CAsset(const CAsset& _origin) = delete;             // ���� ������ ����

    template<typename T>
    friend class Ptr;

    friend class CAssetMgr;
};

