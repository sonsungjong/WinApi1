#include "pch.h"
#include "ConstBuffer.h"
#include "Device.h"

int CConstBuffer::create(size_t _sizeBuffer, CB_TYPE _enumCBType)
{
	m_type = _enumCBType;

	m_descCB.ByteWidth = static_cast<UINT>(_sizeBuffer);
	m_descCB.MiscFlags = 0;
	m_descCB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;				// ��� ����
	m_descCB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_descCB.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(DEVICE->CreateBuffer(&m_descCB, nullptr, m_pCB.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CConstBuffer::setData(void* _pData)
{
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_pCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);

	memcpy(tSub.pData, _pData, m_descCB.ByteWidth);

	CONTEXT->Unmap(m_pCB.Get(), 0);
}

void CConstBuffer::binding()
{
	// B �� ������
	CONTEXT->VSSetConstantBuffers((UINT)m_type, 1, m_pCB.GetAddressOf());
}

CConstBuffer::CConstBuffer()
	: m_type(CB_TYPE::END)
	, m_descCB{}
{
}

CConstBuffer::~CConstBuffer()
{
}
