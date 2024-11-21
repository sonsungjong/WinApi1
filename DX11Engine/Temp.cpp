#include "pch.h"
#include "Temp.h"
#include "Device.h"

// ���� ���� ���� ����
ComPtr<ID3D11Buffer> g_VB;
ComPtr<ID3D11InputLayout> g_layout;					// ���� �ϳ��� �����ϴ� Layout ����

// �����ǰ� �÷� 3��
Vtx g_arrVtx[3] = {};

// HLSL (������� �ƴ϶� C++�� �����ϰ� ���̴� �ڵ带 ���������ִ� ����) [.fx���� �Ӽ�����.. ���̴����� /fx, Shader Model 5.0 ����]

// Vertex Shader (������)
ComPtr<ID3DBlob> g_VSBlob;					// �������� ���̴� �ڵ带 ����
ComPtr<ID3D11VertexShader> g_VS;

// Pixel Shader (�ȼ���)
ComPtr<ID3DBlob> g_PSBlob;
ComPtr<ID3D11PixelShader> g_PS;


int TempInit()
{
	// ����͸� -1 ~ 1 ���� �߾��� 0,0���� ����ȭ�Ѵ� [NDC��ǥ]
	g_arrVtx[0].vPos = Vec3(0.f, 1.f, 0.f);
	g_arrVtx[0].vColor = Vec4(0.f, 1.f, 0.f, 1.f);			// ����ȭ�ؼ� 255�� 0.0f~1.0f �� (RGBA)

	g_arrVtx[1].vPos = Vec3(1.f, -1.f, 0.f);
	g_arrVtx[1].vColor = Vec4(0.f, 1.f, 0.f, 1.f);			// ����ȭ�ؼ� 255�� 0.0f~1.0f ��

	g_arrVtx[2].vPos = Vec3(-1.f, -1.f, 0.f);
	g_arrVtx[2].vColor = Vec4(0.f, 0.f, 1.f, 1.f);			// ����ȭ�ؼ� 255�� 0.0f~1.0f ��

	// ���� ���� ����
	D3D11_BUFFER_DESC VBDesc = {};

	VBDesc.ByteWidth = sizeof(Vtx) * 3;
	VBDesc.MiscFlags = 0;

	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						// �뵵

	// ����� ���� ����
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA SubDesc = {};
	SubDesc.pSysMem = g_arrVtx;
	if (FAILED(DEVICE->CreateBuffer(&VBDesc, &SubDesc, g_VB.GetAddressOf()))) {
		return E_FAIL;
	}
	// ���ؽ� ���̴�

	// ���� ���� Layout ���� ����
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

	// DEVICE->CreateInputLayout(LayoutDesc, 2, );

	// �ȼ� ���̴�

	return S_OK;
}

void TempRelease()
{

}

void TempTick()
{

}

void TempRender()
{

}
