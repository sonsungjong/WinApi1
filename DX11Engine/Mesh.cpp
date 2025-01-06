#include "pch.h"
#include "Mesh.h"
#include "Device.h"

int CMesh::create(Vtx* _pVtxSysMem, size_t _nVtxCount, UINT* _pIdxSysMem, size_t _nIdxCount)
{
	m_nVtxCount = (UINT)_nVtxCount;
	m_nIdxCount = (UINT)_nIdxCount;

	// ���� ���� ����
	m_descVB.ByteWidth = static_cast<UINT>(sizeof(Vtx) * _nVtxCount);
	m_descVB.MiscFlags = 0;

	m_descVB.BindFlags = D3D11_BIND_VERTEX_BUFFER;						// �뵵

	// ����� ���� ���� ���� (0 : ����)
	m_descVB.CPUAccessFlags = 0;
	m_descVB.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA SubDesc = {};
	SubDesc.pSysMem = _pVtxSysMem;

	if (FAILED(DEVICE->CreateBuffer(&m_descVB, &SubDesc, m_pVB.GetAddressOf()))) {
		return E_FAIL;
	}

	// �ε��� ���� ó��
	m_descIB.ByteWidth = static_cast<UINT>(sizeof(UINT) * _nIdxCount);				// 6���� ���� �ε����� ��Ī
	m_descIB.MiscFlags = 0;

	m_descIB.BindFlags = D3D11_BIND_INDEX_BUFFER;			// �ε��� ���۷� ����

	// ����� ���� ����
	m_descIB.CPUAccessFlags = 0;			// �ε��� ���۴� ������ ���� ��� 0
	m_descIB.Usage = D3D11_USAGE_DEFAULT;

	SubDesc = {};
	SubDesc.pSysMem = _pIdxSysMem;
	if (FAILED(DEVICE->CreateBuffer(&m_descIB, &SubDesc, m_pIB.GetAddressOf()))) {
		return E_FAIL;
	}

	// ���������� �޸𸮰� �����Ǳ� ���� ��������� ���� ���
	m_pVtxSysMem = new Vtx[m_nVtxCount];
	memcpy(m_pVtxSysMem, _pVtxSysMem, sizeof(Vtx) * m_nVtxCount);

	m_pIdxSysMem = new UINT[m_nIdxCount];
	memcpy(m_pIdxSysMem, _pIdxSysMem, sizeof(UINT) * m_nIdxCount);

	return S_OK;
}

void CMesh::binding()
{
	UINT stride = sizeof(Vtx);
	UINT nOffset = 0U;
	CONTEXT->IASetVertexBuffers(0, 1, m_pVB.GetAddressOf(), &stride, &nOffset);
	CONTEXT->IASetIndexBuffer(m_pIB.Get(), DXGI_FORMAT_R32_UINT, 0);		// �ε��� ���۸� ���޽�Ű��
}

void CMesh::render()
{
	binding();

	CONTEXT->DrawIndexed(m_nIdxCount, 0, 0);							// �ε��� ���۸� ���� 6�� ���� �׸���
}

// �θ��� �����ڸ� �ʱ�ȭ
CMesh::CMesh()
	: CAsset(ASSET_TYPE::MESH)
	, m_descVB{}
	, m_nVtxCount(0)
	, m_pVtxSysMem(nullptr)
	, m_descIB{}
	, m_nIdxCount(0)
	, m_pIdxSysMem(nullptr)
{

}

CMesh::~CMesh()
{
	if (m_pVtxSysMem != nullptr) {
		delete[] m_pVtxSysMem;
	}
	if (m_pIdxSysMem != nullptr) {
		delete[] m_pIdxSysMem;
	}
}
