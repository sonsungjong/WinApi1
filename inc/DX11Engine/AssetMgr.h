#pragma once
#include "singleton.h"

#include "PathMgr.h"
#include "assets.h"

class CAssetMgr :
    public CSingleton<CAssetMgr>
{
    SINGLE(CAssetMgr)
private:
    std::map<std::wstring, Ptr<CAsset>> m_mapAsset[(UINT)ASSET_TYPE::END];

public:
    void init();
    void createDefaultMesh();
    void createDefaultTexture();
    void createDefaultMaterial();
    void createDefaultGraphicShader();
    void createDefaultComputeShader();

public:
    template<typename T>
    Ptr<T> load(const std::wstring& _strKey, const std::wstring& _strRelativePath);

    template<typename T>
    Ptr<T> FindAsset(const std::wstring& _strKey);                                  // 검색하고
    
    template<typename T>
    void AddAsset(const std::wstring& _strKey, Ptr<T> pAsset);              // 없으면 추가
};

// std::is_same_v
// 변수 템플릿 (타입을 정해줘야 변수가 생성됨... 특수화가 가능[특정 타입일 경우 변칙을 줄 수 있음])
// constexpr : 엄격한 상수화 (컴파일 타임에 상수화 보장)
//template<typename T1, typename T2>
//constexpr bool mybool = false;
//template<typename T1>
//constexpr bool mybool<T1, T1> = true;

template<typename T>
ASSET_TYPE getAssetType()
{
    // Run Time Type Information (RTTI) : 실시간 타입을 알아내는 정보 (상속과 가상함수가 존재해야 가능)
    //const type_info& info = typeid(T);
    //if (info.hash_code() == typeid(CMesh).hash_code()) { return ASSET_TYPE::MESH; }                   // T의 타입 해시코드     (if는 런타임 시간을 소요함)
    //else if (info.hash_code() == typeid(CGraphicShader).hash_code()) {
    //    return ASSET_TYPE::GRAPHICS_SHADER;
    //}
    //else if (info.hash_code() == typeid(CComputeShader).hash_code()) {
    //    return ASSET_TYPE::COMPUTE_SHADER;
    //}

    //if constexpr (mybool<T, CMesh>)
    //{
    //    return ASSET_TYPE::MESH;
    //}
    //if constexpr (mybool<T, CGraphicShader>)
    //{
    //    return ASSET_TYPE::GRAPHICS_SHADER;
    //}
    //if constexpr (mybool<T, CComputeShader>)
    //{
    //    return ASSET_TYPE::COMPUTE_SHADER;
    //}

    // if constexpr : 컴파일 타임에서 제거함
    if constexpr (std::is_same_v<T, CMesh>)
    {
        return ASSET_TYPE::MESH;
    }
    if constexpr (std::is_same_v<T, CGraphicShader>)
    {
        return ASSET_TYPE::GRAPHICS_SHADER;
    }
    if constexpr (std::is_same_v<T, CComputeShader>)
    {
        return ASSET_TYPE::COMPUTE_SHADER;
    }
    if constexpr (std::is_same_v<T, CTexture>)
    {
        return ASSET_TYPE::TEXTURE;
    }
}

template<typename T>
inline Ptr<T> CAssetMgr::load(const std::wstring& _strKey, const std::wstring& _strRelativePath)
{
    Ptr<CAsset> pAsset = FindAsset<T>(_strKey).Get();

    // 등록되어 있었다면
    if (nullptr != pAsset.Get()) {
        return (T*)pAsset.Get();
    }

    std::wstring strFullPath = CPathMgr::getInstance()->getResPath();
    strFullPath += _strRelativePath;

    // 등록된 적이 없다면
    pAsset = new T;
    if (FAILED(pAsset->load(strFullPath)))
    {
        MessageBox(NULL, strFullPath.c_str(), L"에셋 로딩 실패", MB_OK);
        return nullptr;
    }

    return (T*)pAsset.Get();
}

template<typename T>
inline Ptr<T> CAssetMgr::FindAsset(const std::wstring& _strKey)
{
    ASSET_TYPE type = getAssetType<T>();

    std::map<std::wstring, Ptr<CAsset>>::iterator iter = m_mapAsset[(UINT)type].find(_strKey);

    if (iter == m_mapAsset[(UINT)type].end())
    {
        return nullptr;
    }

    T* pAsset = dynamic_cast<T*>(iter->second.Get());               // 다운캐스팅으로 정확한 자료형이 들어왔는지 체크한다
    return pAsset;

}

template<typename T>
inline void CAssetMgr::AddAsset(const std::wstring& _strKey, Ptr<T> _pAsset)
{
    Ptr<T> pFindAsset = FindAsset<T>(_strKey);          // 잘못된 키인지 검사한다.

    assert(pFindAsset.Get() == nullptr);                // 0 에 반응하는 assert

    ASSET_TYPE type = getAssetType<T>();

    m_mapAsset[(UINT)type].insert(make_pair(_strKey, _pAsset.Get()));
    _pAsset->m_key = _strKey;
}