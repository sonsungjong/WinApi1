#include "pch.h"
#include "Temp.h"
#include "Device.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "PathMgr.h"
#include "AssetMgr.h"

#include "Mesh.h"

#include "ConstBuffer.h"
#include "GraphicShader.h"

//Ptr<CMesh> g_pRectMesh = nullptr;
//Ptr<CMesh> g_pCircleMesh = nullptr;

// ���� ���� ���� ����
//ComPtr<ID3D11Buffer> g_VB;

// ���� ���� ������ ����� ������ ����Ű�� �ε��� ������ ������ ���� (��ø��ġ ����ȭ)
//ComPtr<ID3D11Buffer> g_IB;

// �������(Constant Buffer) ��ü�� ��ġ, ũ��, ȸ���� ������ ���� (�̵����� ����)
//ComPtr<ID3D11Buffer> g_CB;

// ���� �ϳ��� �����ϴ� Layout ����
//ComPtr<ID3D11InputLayout> g_layout;					

// ����(������,�÷�)
//const int g_vtx_array_count = VTX_RECT_ARRAY_COUNT;
//const int g_idx_array_count = IDX_RECT_ARRAY_COUNT;
//Vtx g_arrVtx[g_vtx_array_count] = {};

// ��ü�� ��ġ�� + ũ��, ȸ��
//Vec3 g_ObjectPos;
tTransform g_Trans = {};

// HLSL (������� �ƴ϶� C++�� �����ϰ� ���̴� �ڵ带 ���������ִ� ����) [.fx���� �Ӽ�����.. ���̴����� /fx, Shader Model 5.0 ����]
//Ptr<CGraphicShader> g_shader = nullptr;

// Vertex Shader (������)
//ComPtr<ID3DBlob> g_VSBlob;					// �������� ���̴� �ڵ带 ����
//ComPtr<ID3D11VertexShader> g_VS;

// Pixel Shader (�ȼ���)
//ComPtr<ID3DBlob> g_PSBlob;
//ComPtr<ID3D11PixelShader> g_PS;

// Error Blob
//ComPtr<ID3DBlob> g_ErrBlob;


int TempInit()
{
	// ����͸� -1 ~ 1 ���� �߾��� 0,0���� ����ȭ�Ѵ� [NDC��ǥ]
	// [�簢�� �޽�] �������� �ʱ�ȭ
	//g_arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	//g_arrVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 1.f);			// ����ȭ�ؼ� 255�� 0.0f~1.0f �� (RGBA)

	//g_arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	//g_arrVtx[1].vColor = Vec4(0.f, 1.f, 0.f, 1.f);			// ����ȭ�ؼ� 255�� 0.0f~1.0f ��

	//g_arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	//g_arrVtx[2].vColor = Vec4(0.f, 0.f, 1.f, 1.f);			// ����ȭ�ؼ� 255�� 0.0f~1.0f ��

	//g_arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	//g_arrVtx[3].vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	//// �ε��� ���� ����
	//UINT arrIdx[g_idx_array_count] = { 0, 2, 3, 0, 1, 2 };

	//g_pRectMesh = new CMesh;
	//g_pRectMesh->create(g_arrVtx, g_vtx_array_count, arrIdx, g_idx_array_count);

	//// [�� �޽�]
	//float fRadius = 0.5f;
	//UINT nSlice = 360U;
	//float fAngleStep = (2 * XM_PI) / nSlice;

	//std::vector<Vtx> vecVtx;
	//std::vector<UINT> vecIdx;
	//// ������ ���� �־��ش�
	//Vtx v;
	//v.vPos = Vec3(0.f, 0.f, 0.f);
	//v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	//vecVtx.push_back(v);
	//
	//float fAngle = 0.f;
	//for (UINT i = 0U; i <= nSlice; ++i) {
	//	v.vPos = Vec3(cosf(fAngle) * fRadius, sinf(fAngle) * fRadius, 0.f);				// x, y, z
	//	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	//	
	//	vecVtx.push_back(v);

	//	fAngle += fAngleStep;
	//}

	//for (UINT i = 0U; i < nSlice; ++i) {
	//	vecIdx.push_back(0);
	//	vecIdx.push_back(i+2);
	//	vecIdx.push_back(i+1);
	//}

	//g_pCircleMesh = new CMesh;
	//g_pCircleMesh->create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());

	// ��� ���� (Constant Buffer)
	//D3D11_BUFFER_DESC CBDesc = {};
	//CBDesc.ByteWidth = sizeof(tTransform);
	//CBDesc.MiscFlags = 0;
	//CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//CBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//CBDesc.Usage = D3D11_USAGE_DYNAMIC;

	//if (FAILED(DEVICE->CreateBuffer(&CBDesc, nullptr, g_CB.GetAddressOf())))
	//{
	//	return E_FAIL;
	//}

	// ���̴� ����
	//std::wstring strPath = std::wstring(CPathMgr::getInstance()->getContentPath());
	//strPath += L"shader\\std2d.fx";

	//g_shader = new CGraphicShader;
	//g_shader->createVertexShader(strPath, "VS_Std2D");
	//g_shader->createPixelShader(strPath, "PS_Std2D");

	// ���ؽ� ���̴�
	//if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	//	, "VS_Std2D", "vs_5_0", D3DCOMPILE_DEBUG, 0, g_VSBlob.GetAddressOf(), g_ErrBlob.GetAddressOf())))
	//{
	//	if (nullptr != g_ErrBlob) {
	//		::MessageBoxA(GetActiveWindow(), (char*)g_ErrBlob->GetBufferPointer(), "���ؽ� ���̴� ������ ����", MB_OK);
	//	}
	//	else {
	//		::MessageBox(GetActiveWindow(), L"������ ã�� �� �����ϴ�.", L"���ؽ� ���̴� ������ ����", MB_OK);
	//	}
	//	return E_FAIL;
	//}

	//if (FAILED(DEVICE->CreateVertexShader(g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), nullptr, g_VS.GetAddressOf()))) {
	//	return E_FAIL;
	//}

	//// ���� ���� Layout ���� ����
	//D3D11_INPUT_ELEMENT_DESC LayoutDesc[2] = {};				// ��ġ�� ����
	//unsigned int nOffset = 0U;

	//LayoutDesc[0].AlignedByteOffset = nOffset;
	//LayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;					// VEC3 (12byte, float3)
	//LayoutDesc[0].InputSlot = 0;
	//LayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//LayoutDesc[0].InstanceDataStepRate = 0;
	//LayoutDesc[0].SemanticName = "POSITION";
	//LayoutDesc[0].SemanticIndex = 0;															// POSITION0
	//nOffset += 12U;
	//
	//LayoutDesc[1].AlignedByteOffset = nOffset;
	//LayoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;				// VEC4 (16byte, float4)
	//LayoutDesc[1].InputSlot = 0;
	//LayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//LayoutDesc[1].InstanceDataStepRate = 0;
	//LayoutDesc[1].SemanticName = "COLOR";
	//LayoutDesc[1].SemanticIndex = 0;															// COLOR0
	//nOffset += 16U;

	//if (FAILED(DEVICE->CreateInputLayout(LayoutDesc, 2, g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), g_layout.GetAddressOf())))
	//{
	//	return E_FAIL;
	// }

	// �ȼ� ���̴�
	//if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	//	, "PS_Std2D", "ps_5_0", D3DCOMPILE_DEBUG, 0, g_PSBlob.GetAddressOf(), g_ErrBlob.GetAddressOf())))
	//{
	//	if (nullptr != g_ErrBlob) {
	//		::MessageBoxA(GetActiveWindow(), (char*)g_ErrBlob->GetBufferPointer(), "�ȼ� ���̴� ������ ����", MB_OK);
	//	}
	//	else {
	//		::MessageBox(GetActiveWindow(), L"������ ã�� �� �����ϴ�.", L"�ȼ� ���̴� ������ ����", MB_OK);
	//	}
	//	return E_FAIL;
	//}

	//if (FAILED(DEVICE->CreatePixelShader(g_PSBlob->GetBufferPointer(), g_PSBlob->GetBufferSize(), nullptr, g_PS.GetAddressOf()))) 
	//{
	//	return E_FAIL;
	//}

	//g_ObjectPos = Vec3(0.f, 0.f, 0.f);				// ��ü�� ó���� ������

	return S_OK;
}

void TempRelease()
{
	//if (nullptr != g_pRectMesh) {
	//	delete g_pRectMesh;
	//}
	//if (nullptr != g_pCircleMesh) {
	//	delete g_pCircleMesh;
	//}
	//if (nullptr != g_shader) {
	//	delete g_shader;
	//}
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
		g_Trans.Position.y += DT;
	}

	//if (GetAsyncKeyState('S') & 0x8001)
	if (KEY_PRESSED(KEY::S))
	{
		// ������ �������� �ְų� ���������� �迭���� �����Ѵ�
		g_Trans.Position.y -= DT;
	}

	//if (GetAsyncKeyState('A') & 0x8001)
	if (KEY_PRESSED(KEY::A))
	{
		// ������ �������� �ְų� ���������� �迭���� �����Ѵ�
		g_Trans.Position.x -= DT;
	}

	//if (GetAsyncKeyState('D') & 0x8001)
	if (KEY_PRESSED(KEY::D))
	{
		// ������ �������� �ְų� ���������� �迭���� �����Ѵ�
		g_Trans.Position.x += DT;
	}

	// System Memory -> GPU
	CConstBuffer* pCB = CDevice::getInstance()->getConstBuffer(CB_TYPE::TRANSFORM);			// b0 �� ����
	pCB->setData(&g_Trans);

	//D3D11_MAPPED_SUBRESOURCE tSub = {};
	//CONTEXT->Map(g_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	//tTransform trans = {};
	//trans.Position = g_ObjectPos;
	//memcpy(tSub.pData, &trans, sizeof(tTransform));
	//CONTEXT->Unmap(g_CB.Get(), 0);

	pCB->binding();						// ����� �����س��� CB_TYPE::TRANSFORM�� ���ε� b0
	// B0 �� ������ (���ε�)
	//CONTEXT->VSSetConstantBuffers(0, 1, g_CB.GetAddressOf());
}

void TempRender()
{

	//CONTEXT->IASetInputLayout(g_layout.Get());
	//CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		// ���� ������ ���� ������ �˷��ش�

	// ���ؽ� ���̴� ����
	//CONTEXT->VSSetShader(g_VS.Get(), nullptr, 0);

	// �ȼ� ���̴� ����
	//CONTEXT->PSSetShader(g_PS.Get(), nullptr, 0);
	//g_shader->binding();			// ���ؽ� ���̴��� �ȼ� ���̴��� ����
	Ptr<CGraphicShader> pShader = CAssetMgr::getInstance()->FindAsset<CGraphicShader>(L"Std2DShader");
	pShader->binding();

	// Depth Stencil State �� �⺻ �ɼ����� �ҰŶ� ��þ���


	// Draw�� ȣ���ؼ� �׸���
	//CONTEXT->Draw(g_vtx_count, 0);
	//g_pRectMesh->render();
	Ptr<CMesh> pRectMesh = CAssetMgr::getInstance()->FindAsset<CMesh>(L"RectMesh");
	pRectMesh->render();
}

// �ε��� ���� : �簢������ 6�� -> 4�� (��ø ����ȭ)