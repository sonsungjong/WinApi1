#pragma once
#include "Entity.h"
class CConstBuffer
    : public CEntity
{
private:
    ComPtr<ID3D11Buffer> m_pCB;
    D3D11_BUFFER_DESC m_descCB;
    CB_TYPE m_type;

    // �������� ��ȣ�� enum������ �����
    
public:
    int create(size_t _sizeBuffer, CB_TYPE _enumCBType);
    void setData(void* _pData);
    void binding();

    CConstBuffer();
    ~CConstBuffer();
};

