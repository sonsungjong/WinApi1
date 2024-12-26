#pragma once
#include "Entity.h"
#include "enum.h"

// Asset(자원)에 해당하는 모든 클래스의 공통부분을 넣는다

class CAsset 
    : public CEntity
{
private:
    std::wstring m_key;                             // 로딩된 키값
    std::wstring m_relativePath;                // 상대경로

    const ASSET_TYPE m_type;                // Asset의 타입

public:
    const std::wstring& getKey() { return m_key; }
    const std::wstring& getRelativePath() { return m_relativePath; }
    ASSET_TYPE getAssetType() { return m_type; }

    CAsset(ASSET_TYPE _type);
    ~CAsset();
};

