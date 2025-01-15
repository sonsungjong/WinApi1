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

// 정점 정보 저장 버퍼
//ComPtr<ID3D11Buffer> g_VB;

// 정점 버퍼 내에서 사용할 정점을 가리키는 인덱스 정보를 저장할 버퍼 (중첩위치 최적화)
//ComPtr<ID3D11Buffer> g_IB;

// 상수버퍼(Constant Buffer) 물체의 위치, 크기, 회전량 정보를 전달 (이동량만 전달)
//ComPtr<ID3D11Buffer> g_CB;

// 정점 하나를 구성하는 Layout 정보
//ComPtr<ID3D11InputLayout> g_layout;					

// 정점(포지션,컬러)
//const int g_vtx_array_count = VTX_RECT_ARRAY_COUNT;
//const int g_idx_array_count = IDX_RECT_ARRAY_COUNT;
//Vtx g_arrVtx[g_vtx_array_count] = {};

// 물체의 위치값 + 크기, 회전
//Vec3 g_ObjectPos;
tTransform g_Trans = {};

// HLSL (어셈블리가 아니라 C++과 유사하게 쉐이더 코드를 컴파일해주는 형식) [.fx파일 속성에서.. 셰이더형식 /fx, Shader Model 5.0 설정]
//Ptr<CGraphicShader> g_shader = nullptr;

// Vertex Shader (정점당)
//ComPtr<ID3DBlob> g_VSBlob;					// 컴파일한 쉐이더 코드를 저장
//ComPtr<ID3D11VertexShader> g_VS;

// Pixel Shader (픽셀당)
//ComPtr<ID3DBlob> g_PSBlob;
//ComPtr<ID3D11PixelShader> g_PS;

// Error Blob
//ComPtr<ID3DBlob> g_ErrBlob;


int TempInit()
{
	// 모니터를 -1 ~ 1 까지 중앙을 0,0으로 정규화한다 [NDC좌표]
	// [사각형 메시] 정점버퍼 초기화
	//g_arrVtx[0].vPos = Vec3(-0.5f, 0.5f, 0.f);
	//g_arrVtx[0].vColor = Vec4(1.f, 0.f, 0.f, 1.f);			// 정규화해서 255를 0.0f~1.0f 로 (RGBA)

	//g_arrVtx[1].vPos = Vec3(0.5f, 0.5f, 0.f);
	//g_arrVtx[1].vColor = Vec4(0.f, 1.f, 0.f, 1.f);			// 정규화해서 255를 0.0f~1.0f 로

	//g_arrVtx[2].vPos = Vec3(0.5f, -0.5f, 0.f);
	//g_arrVtx[2].vColor = Vec4(0.f, 0.f, 1.f, 1.f);			// 정규화해서 255를 0.0f~1.0f 로

	//g_arrVtx[3].vPos = Vec3(-0.5f, -0.5f, 0.f);
	//g_arrVtx[3].vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	//// 인덱스 버퍼 생성
	//UINT arrIdx[g_idx_array_count] = { 0, 2, 3, 0, 1, 2 };

	//g_pRectMesh = new CMesh;
	//g_pRectMesh->create(g_arrVtx, g_vtx_array_count, arrIdx, g_idx_array_count);

	//// [원 메시]
	//float fRadius = 0.5f;
	//UINT nSlice = 360U;
	//float fAngleStep = (2 * XM_PI) / nSlice;

	//std::vector<Vtx> vecVtx;
	//std::vector<UINT> vecIdx;
	//// 원점을 먼저 넣어준다
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

	// 상수 버퍼 (Constant Buffer)
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

	// 쉐이더 생성
	//std::wstring strPath = std::wstring(CPathMgr::getInstance()->getContentPath());
	//strPath += L"shader\\std2d.fx";

	//g_shader = new CGraphicShader;
	//g_shader->createVertexShader(strPath, "VS_Std2D");
	//g_shader->createPixelShader(strPath, "PS_Std2D");

	// 버텍스 쉐이더
	//if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	//	, "VS_Std2D", "vs_5_0", D3DCOMPILE_DEBUG, 0, g_VSBlob.GetAddressOf(), g_ErrBlob.GetAddressOf())))
	//{
	//	if (nullptr != g_ErrBlob) {
	//		::MessageBoxA(GetActiveWindow(), (char*)g_ErrBlob->GetBufferPointer(), "버텍스 쉐이더 컴파일 오류", MB_OK);
	//	}
	//	else {
	//		::MessageBox(GetActiveWindow(), L"파일을 찾을 수 없습니다.", L"버텍스 쉐이더 컴파일 오류", MB_OK);
	//	}
	//	return E_FAIL;
	//}

	//if (FAILED(DEVICE->CreateVertexShader(g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), nullptr, g_VS.GetAddressOf()))) {
	//	return E_FAIL;
	//}

	//// 정점 구성 Layout 정보 설정
	//D3D11_INPUT_ELEMENT_DESC LayoutDesc[2] = {};				// 위치와 색상
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

	// 픽셀 셰이더
	//if (FAILED(D3DCompileFromFile(strPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
	//	, "PS_Std2D", "ps_5_0", D3DCOMPILE_DEBUG, 0, g_PSBlob.GetAddressOf(), g_ErrBlob.GetAddressOf())))
	//{
	//	if (nullptr != g_ErrBlob) {
	//		::MessageBoxA(GetActiveWindow(), (char*)g_ErrBlob->GetBufferPointer(), "픽셀 쉐이더 컴파일 오류", MB_OK);
	//	}
	//	else {
	//		::MessageBox(GetActiveWindow(), L"파일을 찾을 수 없습니다.", L"픽셀 쉐이더 컴파일 오류", MB_OK);
	//	}
	//	return E_FAIL;
	//}

	//if (FAILED(DEVICE->CreatePixelShader(g_PSBlob->GetBufferPointer(), g_PSBlob->GetBufferSize(), nullptr, g_PS.GetAddressOf()))) 
	//{
	//	return E_FAIL;
	//}

	//g_ObjectPos = Vec3(0.f, 0.f, 0.f);				// 물체를 처음에 원점에

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
	// 이동속도를 시간동기화 처리 (프레임차이 극복)
	float DT = CTimeMgr::getInstance()->getDeltaTime();

	// 키보드 입력으로 삼각형을 움직이게 한다
	//if (GetAsyncKeyState('W') & 0x8001) 
	if(KEY_PRESSED(KEY::W))
	{
		// 이전에 눌린적이 있거나 눌려있으면 배열값을 수정한다
		g_Trans.Position.y += DT;
	}

	//if (GetAsyncKeyState('S') & 0x8001)
	if (KEY_PRESSED(KEY::S))
	{
		// 이전에 눌린적이 있거나 눌려있으면 배열값을 수정한다
		g_Trans.Position.y -= DT;
	}

	//if (GetAsyncKeyState('A') & 0x8001)
	if (KEY_PRESSED(KEY::A))
	{
		// 이전에 눌린적이 있거나 눌려있으면 배열값을 수정한다
		g_Trans.Position.x -= DT;
	}

	//if (GetAsyncKeyState('D') & 0x8001)
	if (KEY_PRESSED(KEY::D))
	{
		// 이전에 눌린적이 있거나 눌려있으면 배열값을 수정한다
		g_Trans.Position.x += DT;
	}

	// System Memory -> GPU
	CConstBuffer* pCB = CDevice::getInstance()->getConstBuffer(CB_TYPE::TRANSFORM);			// b0 로 생성
	pCB->setData(&g_Trans);

	//D3D11_MAPPED_SUBRESOURCE tSub = {};
	//CONTEXT->Map(g_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	//tTransform trans = {};
	//trans.Position = g_ObjectPos;
	//memcpy(tSub.pData, &trans, sizeof(tTransform));
	//CONTEXT->Unmap(g_CB.Get(), 0);

	pCB->binding();						// 멤버에 저장해놓은 CB_TYPE::TRANSFORM로 바인딩 b0
	// B0 에 보낸다 (바인딩)
	//CONTEXT->VSSetConstantBuffers(0, 1, g_CB.GetAddressOf());
}

void TempRender()
{

	//CONTEXT->IASetInputLayout(g_layout.Get());
	//CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		// 원시 도형의 위상 구조를 알려준다

	// 버텍스 쉐이더 셋팅
	//CONTEXT->VSSetShader(g_VS.Get(), nullptr, 0);

	// 픽셀 쉐이더 셋팅
	//CONTEXT->PSSetShader(g_PS.Get(), nullptr, 0);
	//g_shader->binding();			// 버텍스 쉐이더와 픽셀 쉐이더를 셋팅
	Ptr<CGraphicShader> pShader = CAssetMgr::getInstance()->FindAsset<CGraphicShader>(L"Std2DShader");
	pShader->binding();

	// Depth Stencil State 는 기본 옵션으로 할거라 명시안함


	// Draw를 호출해서 그린다
	//CONTEXT->Draw(g_vtx_count, 0);
	//g_pRectMesh->render();
	Ptr<CMesh> pRectMesh = CAssetMgr::getInstance()->FindAsset<CMesh>(L"RectMesh");
	pRectMesh->render();
}

// 인덱스 버퍼 : 사각형정점 6개 -> 4개 (중첩 최적화)