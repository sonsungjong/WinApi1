#include "pch.h"
#include "Temp.h"
#include "Device.h"
#include "TimeMgr.h"
#include "KeyMgr.h"

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

// Error Blob
ComPtr<ID3DBlob> g_ErrBlob;


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
	wchar_t szBuffer[255] = {};
	GetCurrentDirectory(255, szBuffer);

	size_t len = wcslen(szBuffer);
	for (int i = len - 1; i > 0; --i)			// �ڿ������� ���
	{
		if (szBuffer[i] == '\\') {
			szBuffer[i] = '\0';
			break;
		}
	}
	wcscat_s(szBuffer, L"\\content\\shader\\std2d.fx");

	
	if (FAILED(D3DCompileFromFile(szBuffer, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, "VS_Std2D", "vs_5_0", D3DCOMPILE_DEBUG, 0, g_VSBlob.GetAddressOf(), g_ErrBlob.GetAddressOf())))
	{
		if (nullptr != g_ErrBlob) {
			::MessageBoxA(GetActiveWindow(), (char*)g_ErrBlob->GetBufferPointer(), "���ؽ� ���̴� ������ ����", MB_OK);
		}
		else {
			::MessageBox(GetActiveWindow(), L"������ ã�� �� �����ϴ�.", L"���ؽ� ���̴� ������ ����", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateVertexShader(g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), nullptr, g_VS.GetAddressOf()))) {
		return E_FAIL;
	}

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

	if (FAILED(DEVICE->CreateInputLayout(LayoutDesc, 2, g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), g_layout.GetAddressOf())))
	{
		return E_FAIL;
	 }

	// �ȼ� ���̴�
	if (FAILED(D3DCompileFromFile(szBuffer, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, "PS_Std2D", "ps_5_0", D3DCOMPILE_DEBUG, 0, g_PSBlob.GetAddressOf(), g_ErrBlob.GetAddressOf())))
	{
		if (nullptr != g_ErrBlob) {
			::MessageBoxA(GetActiveWindow(), (char*)g_ErrBlob->GetBufferPointer(), "�ȼ� ���̴� ������ ����", MB_OK);
		}
		else {
			::MessageBox(GetActiveWindow(), L"������ ã�� �� �����ϴ�.", L"�ȼ� ���̴� ������ ����", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreatePixelShader(g_PSBlob->GetBufferPointer(), g_PSBlob->GetBufferSize(), nullptr, g_PS.GetAddressOf()))) 
	{
		return E_FAIL;
	}

	

	return S_OK;
}

void TempRelease()
{

}

void TempTick()
{
	// �̵��ӵ��� �ð�����ȭ ó�� (���������� �غ�)
	float DT = CTimeMgr::getInstance()->getDeltaTime();

	// Ű���� �Է����� �ﰢ���� �����̰� �Ѵ�
	//if (GetAsyncKeyState('W') & 0x8001) 
	if(KEY_PRESSED(KEY::W))
	{
		// ������ �������� �ְų� ���������� �迭���� �����Ѵ�
		for (int i = 0; i < 3; ++i) {
			g_arrVtx[i].vPos.y += DT;
		}
	}

	//if (GetAsyncKeyState('S') & 0x8001)
	if (KEY_PRESSED(KEY::S))
	{
		// ������ �������� �ְų� ���������� �迭���� �����Ѵ�
		for (int i = 0; i < 3; ++i) {
			g_arrVtx[i].vPos.y -= DT;
		}
	}

	//if (GetAsyncKeyState('A') & 0x8001)
	if (KEY_PRESSED(KEY::A))
	{
		// ������ �������� �ְų� ���������� �迭���� �����Ѵ�
		for (int i = 0; i < 3; ++i) {
			g_arrVtx[i].vPos.x -= DT;
		}
	}

	//if (GetAsyncKeyState('D') & 0x8001)
	if (KEY_PRESSED(KEY::D))
	{
		// ������ �������� �ְų� ���������� �迭���� �����Ѵ�
		for (int i = 0; i < 3; ++i) {
			g_arrVtx[i].vPos.x += DT;
		}
	}

	// System Memory -> GPU
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(g_VB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);

	memcpy(tSub.pData, g_arrVtx, sizeof(Vtx) * 3);

	CONTEXT->Unmap(g_VB.Get(), 0);
}

void TempRender()
{
	UINT stride = sizeof(Vtx);
	UINT nOffset = 0U;
	CONTEXT->IASetVertexBuffers(0, 1, g_VB.GetAddressOf(), &stride, &nOffset);
	CONTEXT->IASetInputLayout(g_layout.Get());
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		// ���� ������ ���� ������ �˷��ش�

	// ���ؽ� ���̴� ����
	CONTEXT->VSSetShader(g_VS.Get(), nullptr, 0);

	// �ȼ� ���̴� ����
	CONTEXT->PSSetShader(g_PS.Get(), nullptr, 0);

	// Depth Stencil State �� �⺻ �ɼ����� �ҰŶ� ��þ���


	// Draw�� ȣ���ؼ� �׸���
	CONTEXT->Draw(3, 0);
}
