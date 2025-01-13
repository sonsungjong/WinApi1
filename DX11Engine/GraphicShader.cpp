#include "pch.h"
#include "GraphicShader.h"

#include "Device.h"

CGraphicShader::CGraphicShader()
	: CShader(ASSET_TYPE::GRAPHICS_SHADER)						// �ش� Ŭ������ �׷��� ���̴� ���� �θ�����ڿ� �˷���
	, m_enumTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

CGraphicShader::~CGraphicShader()
{
}

int CGraphicShader::createVertexShader(const std::wstring& _wstrFilePath, const std::string& _strVSFuncName)
{
	if (FAILED(D3DCompileFromFile(_wstrFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strVSFuncName.c_str(), "vs_5_0", D3DCOMPILE_DEBUG, 0
		, m_pVertexShaderBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		if (nullptr != m_ErrBlob) {
			::MessageBoxA(GetActiveWindow(), (char*)m_ErrBlob->GetBufferPointer(), "���ؽ� ���̴� ������ ����", MB_OK);
		}
		else {
			::MessageBox(GetActiveWindow(), L"������ ã�� �� �����ϴ�.", L"���ؽ� ���̴� ������ ����", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateVertexShader(m_pVertexShaderBlob->GetBufferPointer(), m_pVertexShaderBlob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()))) {
		return E_FAIL;
	}

	// ������ �����ϴ� Layout ���� ����
	D3D11_INPUT_ELEMENT_DESC LayoutDesc[2] = {};				// ��ġ�� ����
	unsigned int nOffset = 0U;

	LayoutDesc[0].AlignedByteOffset = nOffset;
	LayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;					// VEC3 (12byte, float3)
	LayoutDesc[0].InputSlot = 0;
	LayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[0].InstanceDataStepRate = 0;
	LayoutDesc[0].SemanticName = "POSITION";
	LayoutDesc[0].SemanticIndex = 0;															// POSITION0
	nOffset += 12U;

	LayoutDesc[1].AlignedByteOffset = nOffset;
	LayoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;				// VEC4 (16byte, float4)
	LayoutDesc[1].InputSlot = 0;
	LayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[1].InstanceDataStepRate = 0;
	LayoutDesc[1].SemanticName = "COLOR";
	LayoutDesc[1].SemanticIndex = 0;															// COLOR0
	nOffset += 16U;

	if (FAILED(DEVICE->CreateInputLayout(LayoutDesc, 2, m_pVertexShaderBlob->GetBufferPointer(), m_pVertexShaderBlob->GetBufferSize(), m_pLayout.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

int CGraphicShader::createPixelShader(const std::wstring& _wstrFilePath, const std::string& _strPSFuncName)
{
	if (FAILED(D3DCompileFromFile(_wstrFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strPSFuncName.c_str(), "ps_5_0", D3DCOMPILE_DEBUG, 0
		, m_pPixelShaderBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		if (nullptr != m_ErrBlob) {
			::MessageBoxA(GetActiveWindow(), (char*)m_ErrBlob->GetBufferPointer(), "�ȼ� ���̴� ������ ����", MB_OK);
		}
		else {
			::MessageBox(GetActiveWindow(), L"������ ã�� �� �����ϴ�.", L"�ȼ� ���̴� ������ ����", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreatePixelShader(m_pPixelShaderBlob->GetBufferPointer(), m_pPixelShaderBlob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf())))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CGraphicShader::binding()
{
	CONTEXT->IASetInputLayout(m_pLayout.Get());					// �� ������ ����
	CONTEXT->IASetPrimitiveTopology(m_enumTopology);

	// ���ؽ� ���̴� ����
	CONTEXT->VSSetShader(m_pVertexShader.Get(), nullptr, 0);

	// �ȼ� ���̴� ����
	CONTEXT->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
}