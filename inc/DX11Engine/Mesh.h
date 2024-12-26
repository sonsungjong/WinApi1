#pragma once
#include "Asset.h"
class CMesh :
    public CAsset
{
private:
    ComPtr<ID3D11Buffer> m_pVB;
    D3D11_BUFFER_DESC m_descVB;
    UINT m_nVtxCount;
    Vtx* m_pVtxSysMem;

    ComPtr<ID3D11Buffer> m_pIB;
    D3D11_BUFFER_DESC m_descIB;
    UINT m_nIdxCount;
    UINT* m_pIdxSysMem;

private:
    void binding();

public:
    int create(Vtx* _pVtxSysMem, size_t _nVtxCount, UINT* _pIdxSysMem, size_t _nIdxCount);
    void render();

public:
    CMesh();
    ~CMesh();
};

