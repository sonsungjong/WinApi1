#include "pch.h"
#include "Temp.h"
#include "Device.h"
#include "TimeMgr.h"
#include "KeyMgr.h"

// 정점 정보 저장 버퍼
ComPtr<ID3D11Buffer> g_VB;
ComPtr<ID3D11InputLayout> g_layout;					// 정점 하나를 구성하는 Layout 정보

// 포지션과 컬러 3개
Vtx g_arrVtx[3] = {};

// HLSL (어셈블리가 아니라 C++과 유사하게 쉐이더 코드를 컴파일해주는 형식) [.fx파일 속성에서.. 셰이더형식 /fx, Shader Model 5.0 설정]

// Vertex Shader (정점당)
ComPtr<ID3DBlob> g_VSBlob;					// 컴파일한 쉐이더 코드를 저장
ComPtr<ID3D11VertexShader> g_VS;

// Pixel Shader (픽셀당)
ComPtr<ID3DBlob> g_PSBlob;
ComPtr<ID3D11PixelShader> g_PS;

// Error Blob
ComPtr<ID3DBlob> g_ErrBlob;


int TempInit()
{
	// 모니터를 -1 ~ 1 까지 중앙을 0,0으로 정규화한다 [NDC좌표]
	g_arrVtx[0].vPos = Vec3(0.f, 1.f, 0.f);
	g_arrVtx[0].vColor = Vec4(0.f, 1.f, 0.f, 1.f);			// 정규화해서 255를 0.0f~1.0f 로 (RGBA)

	g_arrVtx[1].vPos = Vec3(1.f, -1.f, 0.f);
	g_arrVtx[1].vColor = Vec4(0.f, 1.f, 0.f, 1.f);			// 정규화해서 255를 0.0f~1.0f 로

	g_arrVtx[2].vPos = Vec3(-1.f, -1.f, 0.f);
	g_arrVtx[2].vColor = Vec4(0.f, 0.f, 1.f, 1.f);			// 정규화해서 255를 0.0f~1.0f 로

	// 정점 버퍼 생성
	D3D11_BUFFER_DESC VBDesc = {};

	VBDesc.ByteWidth = sizeof(Vtx) * 3;
	VBDesc.MiscFlags = 0;

	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						// 용도

	// 덮어쓰기 가능 설정
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA SubDesc = {};
	SubDesc.pSysMem = g_arrVtx;
	if (FAILED(DEVICE->CreateBuffer(&VBDesc, &SubDesc, g_VB.GetAddressOf()))) {
		return E_FAIL;
	}
	// 버텍스 쉐이더
	wchar_t szBuffer[255] = {};
	GetCurrentDirectory(255, szBuffer);

	size_t len = wcslen(szBuffer);
	for (int i = len - 1; i > 0; --i)			// 뒤에서부터 출발
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
			::MessageBoxA(GetActiveWindow(), (char*)g_ErrBlob->GetBufferPointer(), "버텍스 쉐이더 컴파일 오류", MB_OK);
		}
		else {
			::MessageBox(GetActiveWindow(), L"파일을 찾을 수 없습니다.", L"버텍스 쉐이더 컴파일 오류", MB_OK);
		}
		return E_FAIL;
	}

	if (FAILED(DEVICE->CreateVertexShader(g_VSBlob->GetBufferPointer(), g_VSBlob->GetBufferSize(), nullptr, g_VS.GetAddressOf()))) {
		return E_FAIL;
	}

	// 정점 구성 Layout 정보 설정
	D3D11_INPUT_ELEMENT_DESC LayoutDesc[2] = {};				// 위치와 색상
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

	// 픽셀 셰이더
	if (FAILED(D3DCompileFromFile(szBuffer, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, "PS_Std2D", "ps_5_0", D3DCOMPILE_DEBUG, 0, g_PSBlob.GetAddressOf(), g_ErrBlob.GetAddressOf())))
	{
		if (nullptr != g_ErrBlob) {
			::MessageBoxA(GetActiveWindow(), (char*)g_ErrBlob->GetBufferPointer(), "픽셀 쉐이더 컴파일 오류", MB_OK);
		}
		else {
			::MessageBox(GetActiveWindow(), L"파일을 찾을 수 없습니다.", L"픽셀 쉐이더 컴파일 오류", MB_OK);
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
	// 이동속도를 시간동기화 처리 (프레임차이 극복)
	float DT = CTimeMgr::getInstance()->getDeltaTime();

	// 키보드 입력으로 삼각형을 움직이게 한다
	//if (GetAsyncKeyState('W') & 0x8001) 
	if(KEY_PRESSED(KEY::W))
	{
		// 이전에 눌린적이 있거나 눌려있으면 배열값을 수정한다
		for (int i = 0; i < 3; ++i) {
			g_arrVtx[i].vPos.y += DT;
		}
	}

	//if (GetAsyncKeyState('S') & 0x8001)
	if (KEY_PRESSED(KEY::S))
	{
		// 이전에 눌린적이 있거나 눌려있으면 배열값을 수정한다
		for (int i = 0; i < 3; ++i) {
			g_arrVtx[i].vPos.y -= DT;
		}
	}

	//if (GetAsyncKeyState('A') & 0x8001)
	if (KEY_PRESSED(KEY::A))
	{
		// 이전에 눌린적이 있거나 눌려있으면 배열값을 수정한다
		for (int i = 0; i < 3; ++i) {
			g_arrVtx[i].vPos.x -= DT;
		}
	}

	//if (GetAsyncKeyState('D') & 0x8001)
	if (KEY_PRESSED(KEY::D))
	{
		// 이전에 눌린적이 있거나 눌려있으면 배열값을 수정한다
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
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		// 원시 도형의 위상 구조를 알려준다

	// 버텍스 쉐이더 셋팅
	CONTEXT->VSSetShader(g_VS.Get(), nullptr, 0);

	// 픽셀 쉐이더 셋팅
	CONTEXT->PSSetShader(g_PS.Get(), nullptr, 0);

	// Depth Stencil State 는 기본 옵션으로 할거라 명시안함


	// Draw를 호출해서 그린다
	CONTEXT->Draw(3, 0);
}
