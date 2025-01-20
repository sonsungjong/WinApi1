#include "pch.h"
#include "ConstBuffer.h"

#include "Device.h"

CConstBuffer::CConstBuffer()
	: m_type(CB_TYPE::END)
	, m_descCB{}
{
}

CConstBuffer::~CConstBuffer()
{
}

int CConstBuffer::create(size_t _sizeBuffer, CB_TYPE _enumCBType)
{
	m_type = _enumCBType;

	m_descCB.ByteWidth = static_cast<UINT>(_sizeBuffer);
	m_descCB.MiscFlags = 0;
	m_descCB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;				// 상수 버퍼
	m_descCB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				// 상수 버퍼는 WRITE
	m_descCB.Usage = D3D11_USAGE_DYNAMIC;									// 상수 버퍼는 DYNAMIC

	if (FAILED(DEVICE->CreateBuffer(&m_descCB, nullptr, m_pCB.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

// 주소값을 받아서
void CConstBuffer::setData(void* _pData)
{
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_pCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);

	memcpy(tSub.pData, _pData, m_descCB.ByteWidth);				// 알려준 주소로부터 32바이트 복사해서 넣는다

	CONTEXT->Unmap(m_pCB.Get(), 0);
}

void CConstBuffer::binding()
{
	// B0 에 보낸다
	CONTEXT->VSSetConstantBuffers((UINT)m_type, 1, m_pCB.GetAddressOf());		// m_type == 0 (CB_TYPE::TRANSFORM)
	CONTEXT->DSSetConstantBuffers((UINT)m_type, 1, m_pCB.GetAddressOf());		// m_type == 0 (CB_TYPE::TRANSFORM)
	CONTEXT->HSSetConstantBuffers((UINT)m_type, 1, m_pCB.GetAddressOf());		// m_type == 0 (CB_TYPE::TRANSFORM)
	CONTEXT->GSSetConstantBuffers((UINT)m_type, 1, m_pCB.GetAddressOf());		// m_type == 0 (CB_TYPE::TRANSFORM)
	CONTEXT->PSSetConstantBuffers((UINT)m_type, 1, m_pCB.GetAddressOf());		// m_type == 0 (CB_TYPE::TRANSFORM)
}


