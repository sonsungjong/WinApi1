#pragma once
#include "Entity.h"

// Asset(자원)에 해당하는 모든 클래스의 공통부분을 넣는다

class CAsset 
    : public CEntity
{
private:
    std::wstring m_key;                             // 로딩된 키값
    std::wstring m_relativePath;                // 상대경로
    const ASSET_TYPE m_type;                // Asset의 타입
    int m_nRefCount;                                // 참조 카운트



    void addRef() { m_nRefCount++; }
    void release() { 
        m_nRefCount--;
        if (m_nRefCount <= 0) {
            delete this;                // 스스로를 삭제 (소멸자 호출)
        }
    }

    virtual int load(const std::wstring& _strFilePath) = 0;
    virtual int save(const std::wstring& _strFilePath) = 0;

public:
    const std::wstring& getKey() { return m_key; }
    const std::wstring& getRelativePath() { return m_relativePath; }
    ASSET_TYPE getAssetType() { return m_type; }

    CAsset(ASSET_TYPE _type);
    ~CAsset();
    CAsset(const CAsset& _origin) = delete;             // 복사 생성자 삭제


    template<typename T>
    friend class Ptr;

    friend class CAssetMgr;                 // CAssetMgr 에게는 private 멤버도 허용을 한다
};

