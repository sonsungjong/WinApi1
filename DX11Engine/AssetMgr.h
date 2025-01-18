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
    Ptr<T> FindAsset(const std::wstring& _strKey);                                  // �˻��ϰ�
    
    template<typename T>
    void AddAsset(const std::wstring& _strKey, Ptr<T> pAsset);              // ������ �߰�
};

// std::is_same_v
// ���� ���ø� (Ÿ���� ������� ������ ������... Ư��ȭ�� ����[Ư�� Ÿ���� ��� ��Ģ�� �� �� ����])
// constexpr : ������ ���ȭ (������ Ÿ�ӿ� ���ȭ ����)
//template<typename T1, typename T2>
//constexpr bool mybool = false;
//template<typename T1>
//constexpr bool mybool<T1, T1> = true;

template<typename T>
ASSET_TYPE getAssetType()
{
    // Run Time Type Information (RTTI) : �ǽð� Ÿ���� �˾Ƴ��� ���� (��Ӱ� �����Լ��� �����ؾ� ����)
    //const type_info& info = typeid(T);
    //if (info.hash_code() == typeid(CMesh).hash_code()) { return ASSET_TYPE::MESH; }                   // T�� Ÿ�� �ؽ��ڵ�     (if�� ��Ÿ�� �ð��� �ҿ���)
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

    // if constexpr : ������ Ÿ�ӿ��� ������
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

    // ��ϵǾ� �־��ٸ�
    if (nullptr != pAsset.Get()) {
        return (T*)pAsset.Get();
    }

    std::wstring strFullPath = CPathMgr::getInstance()->getResPath();
    strFullPath += _strRelativePath;

    // ��ϵ� ���� ���ٸ�
    pAsset = new T;
    if (FAILED(pAsset->load(strFullPath)))
    {
        MessageBox(NULL, strFullPath.c_str(), L"���� �ε� ����", MB_OK);
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

    T* pAsset = dynamic_cast<T*>(iter->second.Get());               // �ٿ�ĳ�������� ��Ȯ�� �ڷ����� ���Դ��� üũ�Ѵ�
    return pAsset;

}

template<typename T>
inline void CAssetMgr::AddAsset(const std::wstring& _strKey, Ptr<T> _pAsset)
{
    Ptr<T> pFindAsset = FindAsset<T>(_strKey);          // �߸��� Ű���� �˻��Ѵ�.

    assert(pFindAsset.Get() == nullptr);                // 0 �� �����ϴ� assert

    ASSET_TYPE type = getAssetType<T>();

    m_mapAsset[(UINT)type].insert(make_pair(_strKey, _pAsset.Get()));
    _pAsset->m_key = _strKey;
}